// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

///////////////////////////////////////////////////////////
// librealsense tutorial #2 - Accessing multiple streams //
///////////////////////////////////////////////////////////

// First include the librealsense C++ header file
#include <librealsense/rs.hpp>
#include <cstdio>
#include <GLUT/glut.h>
// Also include GLFW to allow for graphical display
#include <glm.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
using namespace cv;
using namespace std;

typedef struct {
    unsigned char R, G, B;  /* Red, Green, Blue */
} Pixel;
typedef struct {
    Pixel *pPixel;
    int xRes, yRes;
} ColorImage;

int old_rot_x=0;   //剛按下滑鼠時的視窗座標
int old_rot_y=0;

int rot_x=0;      //拖曳後的相對座標，用這決定要旋轉幾度
int rot_y=0;

int record_x=0;      //紀錄上一次旋轉的角度
int record_y=0;
void setMaterial_RGB(float R, float G, float B);
void drawOBJ(GLMmodel *obj);
void WindowSize(int , int );            //負責視窗及繪圖內容的比例
void Keyboard(unsigned char , int, int );   //獲取鍵盤輸入
void Mouse(int , int , int , int );         //獲取滑鼠按下和放開時的訊息
void MotionMouse(int , int );            //獲取滑鼠按下期間的訊息
void Display(void);
GLMmodel *TFb = NULL; //香蕉先生
ColorImage texture;
GLuint  textureID;
void clearColorImage(Pixel background, ColorImage *image)
{
    int i;
    
    if (! image->pPixel) return;
    for (i=0; i<image->xRes*image->yRes; i++) image->pPixel[i] = background;
}
void initColorImage(int xSize, int ySize, ColorImage *image)
{
    Pixel p = {0,0,0};
    image->xRes = xSize;
    image->yRes = ySize;
    image->pPixel = (Pixel*) malloc(sizeof(Pixel)*xSize*ySize);
    clearColorImage(p, image);
}
void readPPM(char *filename, ColorImage *image)
{
    FILE *inFile = fopen(filename, "rb");
    char buffer[1024];
    int xRes, yRes;
    
    assert(inFile); /* die if file can't be opened */
    
    fgets(buffer, 1024, inFile);
    if (0 != strncmp(buffer, "P6", 2)) {
        printf("Sorry, only P6 format is currently supported for PPM files.\n");
        exit(1);
    }
    
    fgets(buffer, 1024, inFile);
    while ('#' == buffer[0]) {  // skip the comment lines
        fgets(buffer, 1024, inFile);
    }
    
    sscanf(buffer, "%d %d", &xRes, &yRes);
    printf("xRes=%d, yRes = %d\n", xRes, yRes);
    initColorImage(xRes, yRes, image);
    
    fgets(buffer, 1024, inFile); // skip the remaining header lines
    
    fread(image->pPixel, 1, 3*image->xRes*image->yRes, inFile );
    fclose(inFile);
}
Mat SobelEdgeDetect(Mat inputImage)
{
    Mat SobelImage; //test
    if(inputImage.type() == CV_8UC3)
    {
        cvtColor(inputImage, inputImage, CV_BGR2GRAY);
    }
    GaussianBlur(inputImage, inputImage, Size(3, 3), 0, 0);
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Sobel(inputImage, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);  //轉成CV_8U
    Sobel(inputImage, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    Mat dst;
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
    threshold(dst, SobelImage, 80, 255, THRESH_BINARY | THRESH_OTSU);
    return SobelImage;
}
int main(int argc, char * argv[]) try
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000,500);         //視窗長寬
    glutInitWindowPosition(600,80);         //視窗左上角的位置
    glutCreateWindow("這裡是視窗標題");      //建立視窗
    glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
    /* setting up the texture */
    glEnable(GL_TEXTURE_2D);
    //glGenTextures(1, textureID);
    readPPM("/Users/TomCruise/Desktop/Banana.ppm", &texture);
    glBindTexture(GL_TEXTURE_2D, textureID);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3,texture.xRes,texture.yRes, GL_RGB, GL_UNSIGNED_BYTE, texture.pPixel);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    TFb = glmReadOBJ( "/Users/TomCruise/Desktop/Banana.obj");
    glmUnitize(TFb);
    //下面五個是用來指定Callback函數
    glutReshapeFunc(WindowSize);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(MotionMouse);
    glutDisplayFunc(Display);
    
    glutMainLoop();
    
    rs::context ctx;
    printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
    if(ctx.get_device_count() == 0) return EXIT_FAILURE;
    
    // This tutorial will access only a single device, but it is trivial to extend to multiple devices
    rs::device * dev = ctx.get_device(0); //裝置
    printf("\nUsing device 0, an %s\n", dev->get_name());
    printf("Serial number: %s\n", dev->get_serial());
    printf("Firmware version: %s\n", dev->get_firmware_version());
    int i = 0;
    // Configure all streams to run at VGA resolution at 60 frames per second
    dev->enable_stream(rs::stream::depth, 640, 480, rs::format::z16, 30);
    dev->enable_stream(rs::stream::color, 640, 480, rs::format::bgr8, 30);
    dev->enable_stream(rs::stream::infrared, 640, 480, rs::format::y8, 30);
    try { dev->enable_stream(rs::stream::infrared2, 640, 480, rs::format::y8, 30); }
    catch(...) { printf("Device does not provide infrared2 stream.\n"); }
    dev->start(); //start streaming

    Mat cameraMatrix, distCoeffs; //相機參數
    // cv::String filename = "/Users/kaofan/Desktop/CameraParas.yml";   //Pro
    cv::String filename = "/Users/TomCruise/Desktop/CameraParas.yml";   //iMac
    Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL);
    float markerLength = 0.05f; // length in meters (?) of a marker
    cv::FileStorage fs;
    fs.open(filename, cv::FileStorage::READ);
    fs["Camera_Matrix"] >> cameraMatrix;
    fs["Distortion_Coefficients"] >> distCoeffs;
    // Open a GLFW window to display our output
    glfwInit();
    GLFWwindow * win = glfwCreateWindow(640,480, "librealsense tutorial #2", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    while(!glfwWindowShouldClose(win))
    {
        // Wait for new frame data
        glfwPollEvents();
        dev->wait_for_frames(); //取得下一幀
        
        //get color & depth data and process by opencv
        rs::intrinsics depth_intr = dev->get_stream_intrinsics(rs::stream::depth);
        cv::Mat depth16( depth_intr.height,depth_intr.width,CV_16U,(void*)dev->get_frame_data(rs::stream::depth) );
        imshow("depth16", depth16);
        cv::Mat depth8u = depth16;
        
        depth8u.convertTo( depth8u, CV_8UC1, 255.0/10000 );
        filterSpeckles(depth8u, -16, 50, 20);
        Mat color(Size(640, 480), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), Mat::AUTO_STEP);
        //imshow("depthSobel",SobelEdgeDetect(depth8u));
        //imshow("colorSobel",SobelEdgeDetect(color));
        Mat imageCopy;
        vector< Vec3d > rvecs, tvecs;
        vector<int>ids;
        vector<vector<Point2f>>corners, rejected;
        cv::aruco::detectMarkers(color, dictionary, corners, ids);
        color.copyTo(imageCopy);
        if(ids.size()>0)
        {
            aruco::drawDetectedMarkers(imageCopy, corners, ids);
            float markerLength = 0.05;
            aruco::estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);
            for(int j = 0;j<ids.size();j++)
            {
                aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[j], tvecs[j], 0.1);
            }
        }
        
        //Mat depth(Size(640, 480), CV_16U, (void*)dev->get_frame_data(rs::stream::depth), Mat::AUTO_STEP); //opencv depth
        
        glClear(GL_COLOR_BUFFER_BIT);
        glPixelZoom(1, -1);
        
        // Display depth data by linearly mapping depth between 0 and 2 meters to the red channel
        glRasterPos2f(-1, 1);
        //glPixelTransferf(GL_RED_SCALE, 0xFFFF * dev->get_depth_scale() / 2.0f);
        glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, depth8u.data);
        //glPixelTransferf(GL_RED_SCALE, 1.0f);
        
        // Display color image as RGB triples
        glRasterPos2f(0, 1);
        glDrawPixels(640, 480, GL_BGR, GL_UNSIGNED_BYTE, imageCopy.data);
        
        // Display infrared image by mapping IR intensity to visible luminance
        glRasterPos2f(-1, 0);
        glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, dev->get_frame_data(rs::stream::infrared));
        
        // Display second infrared image by mapping IR intensity to visible luminance
        if(dev->is_stream_enabled(rs::stream::infrared2))
        {
            glRasterPos2f(0, 0);
            glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, dev->get_frame_data(rs::stream::infrared2));
        }
        
        glfwSwapBuffers(win);
    }
    dev->stop();
    return EXIT_SUCCESS;
}
catch(const rs::error & e) //Realsense例外：裝置被佔用 無法讀取 ..etc
{
    // Method calls against librealsense objects may throw exceptions of type rs::error
    printf("rs::error was thrown when calling %s(%s):\n", e.get_failed_function().c_str(), e.get_failed_args().c_str());
    printf("    %s\n", e.what());
    return EXIT_FAILURE;
}
void Display(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);   //用白色塗背景
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,10.0f,0,0,0,0,1,0);   //視線的座標及方向
    glRotatef((float)rot_y+(float)record_y, 1.0, 0.0, 0.0);//以x軸當旋轉軸
    glRotatef((float)rot_x+(float)record_x, 0.0, 1.0, 0.0);//以y軸當旋轉軸
    glBegin(GL_TRIANGLES);
    glColor3f( 1, 0, 0);glVertex3f( 8.6603, -5, -5); //x,y,z
    glColor3f( 1, 0, 0);glVertex3f(      0, 10, 0); //上方頂點
    glColor3f( 1, 0, 0);glVertex3f(-8.6603, -5, -5);
    
    glColor3f( 0, 1, 0);glVertex3f( 8.6603, -5, -5);
    glColor3f( 0, 1, 0);glVertex3f(      0, 10, 0);
    glColor3f( 0, 1, 0);glVertex3f( 8.6603, -5, 5);
    
    glColor3f( 0, 0, 1);glVertex3f( 8.6603, -5, 5);
    glColor3f( 0, 0, 1);glVertex3f(      0, 10, 0);
    glColor3f( 0, 0, 1);glVertex3f(-8.6603, -5, 5);
    glPushMatrix();
    //透明度
    //glEnable(GL_BLEND);
    //glDepthMask(GL_FALSE);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    setMaterial_RGB( 250, 235, 215 );
    glTranslated(0,-1.1,-2.2);
    //glTranslated(shift[0],shift[1],shift[2]);
    glScaled(0.5,0.5,0.5);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glRotatef(90.0,1.0,0.0,0.0);
    drawOBJ(TFb);
    //glDisable(GL_ALPHA_TEST);
    //glDisable(GL_BLEND);
    //glDepthMask(GL_TRUE);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnd();
    glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
    printf("你所按按鍵的碼是%x\t此時視窗內的滑鼠座標是(%d,%d)\n", key, x, y);
}

void WindowSize(int w, int h)
{
    printf("目前視窗大小為%dX%d\n",w,h);
    glViewport(0, 0, w, h);            //當視窗長寬改變時，畫面也跟著變
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10,10,-10,10,-10,30);      //正交投影
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Mouse(int button, int state, int x, int y)
{
    if(state)
    {
        record_x += x - old_rot_x;
        record_y += y - old_rot_y;
        
        rot_x = 0;   //沒有歸零會有不理想的結果
        rot_y = 0;
    }
    else
    {
        old_rot_x = x;
        old_rot_y = y;
    }
}

void MotionMouse(int x, int y)
{
    rot_x = x - old_rot_x;
    rot_y = y - old_rot_y;
    glutPostRedisplay();
}
void drawOBJ( GLMmodel *objs )
{
    int i, v;
    float *p;
    
    if (! objs) return;
    
    for (i=0; i<objs->numtriangles; i++) {
        // The current triangle is: myObj->triangles[i]
        glBegin(GL_TRIANGLES);
        
        for (v=0; v<3; v++) {
            // Process the normals.
            if (objs->numnormals > 0) {
                p = & objs->normals[ objs->triangles[i].nindices[v]*3 ];
                glNormal3fv(p);
            }
            
            // Process the texture coordinates.
            if (objs->numtexcoords > 0) {
                p = & objs->texcoords[ objs->triangles[i].tindices[v]*2 ];
                glTexCoord2fv(p);
                
            }
            
            // Process the vertices.
            // Assume that the 3 vertices are P[n0], P[n1], P[n2],
            // P[] is equivalent to myObj->vertices, and n0,n1,n2 is related to myObj->triangles[i].vindices[0,1,2]
            p = & objs->vertices[ objs->triangles[i].vindices[v]*3 ];
            
            // Set the RGB based on XYZ.
            // We are assuming that the XYZ are within [-1. 1].
            //glColor3f( p[0]*0.8+0.2, p[1]*0.8+0.2, p[2]*0.8+0.2 );
            //setMaterial_RGB( p[0]*0.5+0.5, p[1]*0.5+0.5, p[2]*0.5+0.5 );
            glVertex3fv( p );
        }
        glEnd();
    }
}
void setMaterial_RGB(float R, float G, float B)
{
    GLfloat ambient[] = {0.1, 0.1, 0.1, 1};
    GLfloat diffuse[] = {0.9, 0.9, 0.9, 1};
    GLfloat specular[] = {0, 0, 0, 1};
    GLfloat shine = 10.0;
    
    ambient[0] *= R;
    ambient[1] *= G;
    ambient[2] *= B;
    diffuse[0] *= R;
    diffuse[1] *= G;
    diffuse[2] *= B;
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
    
    //====== Advanced Usage for Future Experiment ======
    //glColorMaterial(GL_FRONT, GL_DIFFUSE);
    //glEnable(GL_COLOR_MATERIAL);
}

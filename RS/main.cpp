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
#include <GLFW/glfw3.h>
#include <opencv/highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
using namespace cv;
using namespace std;
int old_rot_x=0;   //剛按下滑鼠時的視窗座標
int old_rot_y=0;

int rot_x=0;      //拖曳後的相對座標，用這決定要旋轉幾度
int rot_y=0;

int record_x=0;      //紀錄上一次旋轉的角度
int record_y=0;

void WindowSize(int , int );            //負責視窗及繪圖內容的比例
void Keyboard(unsigned char , int, int );   //獲取鍵盤輸入
void Mouse(int , int , int , int );         //獲取滑鼠按下和放開時的訊息
void MotionMouse(int , int );            //獲取滑鼠按下期間的訊息
void Display(void);
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400,400);         //視窗長寬
    glutInitWindowPosition(600,80);         //視窗左上角的位置
    glutCreateWindow("這裡是視窗標題");      //建立視窗
    
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
    glColor3f( 1, 0, 0);glVertex3f( 8.6603, -5, -3); //x,y,z
    glColor3f( 0, 1, 0);glVertex3f(      0, 10, -3);
    glColor3f( 0, 0, 1);glVertex3f(-8.6603, -5, -3);
    
    glColor3f( 1, 0, 0);glVertex3f( 8.6603, -5, 0);
    glColor3f( 0, 1, 0);glVertex3f(      0, 10, 0);
    glColor3f( 0, 0, 1);glVertex3f(-8.6603, -5, 0);
    
    glColor3f( 1, 0, 0);glVertex3f( 8.6603, -5, 3);
    glColor3f( 0, 1, 0);glVertex3f(      0, 10, 3);
    glColor3f( 0, 0, 1);glVertex3f(-8.6603, -5, 3);
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

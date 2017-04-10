//
//  glfwObject.cpp
//  RS
//
//  Created by Tom Cruise on 2017/3/8.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//
#include "platform.hpp"

#include "glfwObject.hpp"
#include "ObjectLoader.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "glm.h"
#include "Ppm.h"
#include <iostream>

const glm::vec2 SCREEN_SIZE(800, 600);

GLMmodel *Banana = NULL;
GLubyte *BananaSKin = NULL;
GLuint  textureID[1];
GLuint textureBanana;
tdogl::Texture* gTexture = NULL;
tdogl::Program* gProgram = NULL;
GLuint gVAO = 0;
GLuint gVBO = 0;
GLfloat gDegreesRotated = 0.0f;

typedef struct {
    unsigned char R, G, B;  /* Red, Green, Blue */
} Pixel;
typedef struct {
    Pixel *pPixel;
    int xRes, yRes;
} ColorImage;
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
void initTextureID()
{
    ColorImage texture[1];
    readPPM("/Users/kaofan/Desktop/OHAR/Banana.ppm", &texture[0]);
    
    glGenTextures(1, &textureBanana);
    glBindTexture(GL_TEXTURE_2D, textureBanana);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[0].xRes, texture[0].yRes, 0, GL_RGB, GL_UNSIGNED_BYTE, texture[0].pPixel);//(GL_TEXTURE_2D, 3, texture[0].xRes, texture[0].yRes, GL_RGB, GL_UNSIGNED_BYTE, texture[0].pPixel);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}
glfwObject::glfwObject()
{
    glfwObject::RotationX = 0;
    glfwObject::RotationY = 0;
}
glfwObject::glfwObject(string objFileName,string textureFileName)
{
    initTextureID();
}
void glfwObject::LoadTexture() {

    //cout << endl<<"-------" << ResourcePath("wooden-crate.jpg") <<"-------" << endl;
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile("/Users/kaofan/Desktop/OHAR/RS/resources/wooden-crate.jpg");
    bmp.flipVertically();
    gTexture = new tdogl::Texture(bmp);
}
void glfwObject::LoadCube()
{
    // make and bind the VAO
    cout <<"++++++++"<< gVAO <<"++++++++" << endl;
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    // make and bind the VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    
    // Make a cube out of triangles (two triangles per side)
    GLfloat vertexData[] = {
        //  X     Y     Z       U     V
        // bottom
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        
        // top
        -1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        
        // front
        -1.0f,-1.0f, 1.0f,   1.0f, 0.0f,
        1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        
        // back
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
        1.0f, 1.0f,-1.0f,   1.0f, 1.0f,
        
        // left
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        
        // right
        1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
        1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    
    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vert"));
    glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);
    
    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vertTexCoord"));
    glVertexAttribPointer(gProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
    
    // unbind the VAO
    glBindVertexArray(0);
}
float glfwObject::getRotationX()
{
    return RotationX;
}
float glfwObject::getRotationY()
{
    return RotationY;
}
void glfwObject::glfwDrawTorus(int numMajor, int numMinor, float majorRadius, float minorRadius)
{
    static double PI = 3.1415926535897932384626433832795;
    
    double majorStep = 2.0 * PI / numMajor;
    double minorStep = 2.0 * PI / numMinor;
    
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    for (int i = 0; i < numMajor; ++i) {
        double a0 = i * majorStep;
        double a1 = a0 + majorStep;
        GLdouble x0 = cos(a0);
        GLdouble y0 = sin(a0);
        GLdouble x1 = cos(a1);
        GLdouble y1 = sin(a1);
        
        glBegin(GL_TRIANGLE_STRIP);
        
        for (int j = 0; j <= numMinor; ++j) {
            double b = j * minorStep;
            GLdouble c = cos(b);
            GLdouble r = minorRadius * c + majorRadius;
            GLdouble z = minorRadius * sin(b);
            
            glNormal3d(x0 * c, y0 * c, z / minorRadius);
            glTexCoord2d(i / (GLdouble) numMajor, j / (GLdouble) numMinor);
            glVertex3d(x0 * r, y0 * r, z);
            
            glNormal3d(x1 * c, y1 * c, z / minorRadius);
            glTexCoord2d((i + 1) / (GLdouble) numMajor, j / (GLdouble) numMinor);
            glVertex3d(x1 * r, y1 * r, z);
        }
        
        glEnd();
    }
}
void glfwObject::renderMesh(cv::Mat rotateMatrix,cv::Mat translationVector)
{
    initTextureID();
    glPushMatrix();
    
    glLoadIdentity();//移動中心
    //glBindTexture(GL_TEXTURE_2D, textureID[0]);
    //glRotatef(glfwObject::getRotationX()/*+(float)record_x*/, 0.0, 1.0, 0.0);//以y軸當旋轉軸
    //glRotatef(glfwObject::getRotationY()/*+(float)record_y*/, 1.0, 0.0, 0.0);//以x軸當旋轉軸
    cv::Mat viewMatrix(4, 4, CV_64F);
    
    for(unsigned int row=0; row<3; ++row)
    {
        for(unsigned int col=0; col<3; ++col)
        {
            viewMatrix.at<double>(row, col) = rotateMatrix.at<double>(row, col);
            if (row > 0)
            {
                viewMatrix.at<double>(row, col) = -viewMatrix.at<double>(row, col);//!y,z數值
            }
        }
        viewMatrix.at<double>(row, 3) = translationVector.at<double>(0, row);
        viewMatrix.at<double>(row, 3) = 0;
    }
    for(int i = 0;i<3;i++)
    {
        viewMatrix.at<double>(3, i) = 0;
    }
    viewMatrix.at<double>(3, 3) = 2.0f; //縮放（數值越大圖越小）
    
    
    
    cv::Mat glViewMatrix = cv::Mat::zeros(4, 4, CV_64F);
    cv::transpose(viewMatrix , glViewMatrix);
    glMatrixMode(GL_MODELVIEW);
    //glViewMatrix.at<double>(1,0) = -(glViewMatrix.at<double>(1,0));
    glLoadMatrixd(&glViewMatrix.at<double>(0, 0));
    for (int i = 0;i<4;i++)
    {
        for(int j = 0;j<4;j++)
        {
            cout << "'" << i <<"'"<<glViewMatrix.at<double>(i,j) << ",";
        }
        cout << endl;
    }
    glBegin(GL_LINES);
    glColor4ub(255,0,0,255);
    glVertex3f(0,0,0);
    glVertex3f(0.75,0,0);
    
    glColor4ub(0,0,255,255);
    glVertex3f(0,0,0);
    glVertex3f(0,0,0.75);
    
    glColor4ub(0,255,0,255);
    glVertex3f(0,0,0);
    glVertex3f(0,0.75,0);
    
    glEnd();
    glColor4ub(255,255,0,255);
    //glBindTexture(GL_TEXTURE_2D, textureBanana);
    std::vector<float> vertices, normals;
    
    GLuint list_id; //obj list
    Banana = glmReadOBJ("/Users/kaofan/Desktop/OHAR/Banana.obj");
    glmUnitize(Banana);
    glmDraw(Banana, GLM_SMOOTH | GLM_MATERIAL);
    //list_id = glmList(Banana, GLM_MATERIAL | GLM_SMOOTH);
    //glCallList(list_id);    //顯示list中obj
    //glfwObject::glfwDrawTorus(10, 10, 0.5, .2);
    
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}
void glfwObject::mouseMoveHanding(double xpos, double ypos)
{
    glfwObject::RotationX = glfwObject::RecordedX - xpos;
    glfwObject::RotationY = glfwObject::RecordedY - ypos;
}
void glfwObject::drawObj( GLMmodel *objs )
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

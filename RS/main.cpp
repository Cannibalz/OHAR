// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

///////////////////////////////////////////////////////////
// librealsense tutorial #2 - Accessing multiple streams //
///////////////////////////////////////////////////////////

// First include the librealsense C++ header file
#include <librealsense/rs.hpp>
#include <cstdio>
#include <GL/glew.h>
#include <GLUT/glut.h>

// Also include GLFW to allow for graphical display
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "glfwObject.hpp"
#include "realsenseController.hpp"
#include "openCVController.hpp"

#include "tdogl/Program.h"
#include "tdogl/Texture.h"

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
glfwObject renderer = glfwObject();
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) //滑鼠控制
{
    renderer.mouseMoveHanding(xpos, ypos);
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
int main(int argc, char * argv[])
{
    realSense rs;
    ipcv IPCV;
    glfwInit();
    GLFWwindow * win = glfwCreateWindow(1280,480, "librealsense tutorial #2", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    while(!glfwWindowShouldClose(win))
    {
        // Wait for new frame data
        glfwPollEvents();
        rs.waitForNextFrame();
        Mat color = rs.getColorImage();
        imshow("color",color);
        IPCV.RefreshFrame(color);
        IPCV.DetectAndDrawMarkers();
        glClear(GL_COLOR_BUFFER_BIT);
        glPixelZoom(1, -1);
        // Display depth data by linearly mapping depth between 0 and 2 meters to the red channel
        glRasterPos2f(-1, 1);
        //glPixelTransferf(GL_RED_SCALE, 0xFFFF * dev->get_depth_scale() / 2.0f);
        glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, rs.getDepthImage().data);
        //glPixelTransferf(GL_RED_SCALE, 1.0f);
        // Display color image as RGB triples
        glRasterPos2f(0, 1);
        glDrawPixels(640, 480, GL_BGR, GL_UNSIGNED_BYTE, IPCV.getImage().data);
        if(IPCV.getIDs().size()>0 && IPCV.getIDs()[0]==228)
            {
                for(int i = 0;i<IPCV.getIDs().size();i++)
                {
                    cv::Mat oneRvecs(3,1,CV_64FC1);
                    cv::Mat rotMat(4, 4, CV_64F);
                    cv::Mat oneTvecs(3,1,CV_64FC1);
                    for (int a = 0;a<3;a++)
                    {
                        oneRvecs.row(a).col(0) = IPCV.getRvec(i)[a];
                        oneTvecs = IPCV.getTvec(i);
                    }
                    Rodrigues(oneRvecs, rotMat);
                    //renderer.renderMesh(rotMat,oneTvecs);
                    renderer.LoadTexture();
                    renderer.LoadCube();
                }
            }
        glfwSwapBuffers(win);
        //glfwSetCursorPosCallback(win, cursor_position_callback);
    }
    rs.stop();
    return EXIT_SUCCESS;
}

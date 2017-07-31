// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

///////////////////////////////////////////////////////////
// librealsense tutorial #2 - Accessing multiple streams //
///////////////////////////////////////////////////////////

// First include the librealsense C++ header file
//Realsense lib
#include <librealsense/rs.hpp>
//opengl Third-party
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
//OpenCV
#include <opencv/highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
//C++ lib
#include <cstdio>
#include <math.h>
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
//other .hpp
#include "platform.hpp"
#include "glfwObject.hpp"
#include "realsenseController.hpp"
#include "openCVController.hpp"
#include "Program.h"
#include "Texture.h"
using namespace cv;
using namespace std;
// constants

// globals
//GLFWwindow* gWindow = NULL;
//tdogl::Program* gProgram = NULL;
//GLuint gVAO = 0;
//GLuint gVBO = 0;
glfwObject renderer = glfwObject();
int main(int argc, char * argv[])
{
    int a[] = {0,1,2,3};
    for(int i = 0;i<=sizeof(a);i++)
        printf("%d",a[i]);
    realSense rs;
    ipcv IPCV;
    glfwInit();
    GLFWwindow * win = glfwCreateWindow(640,480, "librealsense tutorial #2", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    while(!glfwWindowShouldClose(win))
    {
        // Wait for new frame data
        glfwPollEvents();
        rs.waitForNextFrame();
        Mat color = rs.getColorImage();
        Mat color2 = rs.getColorImage();
        Mat DepthSobel = IPCV.SobelEdgeDetect(rs.getDepthImage());
        Mat colorDS;
        imshow("d",rs.getDepthImage());
        imshow("c2",rs.getAlignedC2D());
        IPCV.RefreshFrame(color);
        IPCV.DetectAndDrawMarkers();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPixelZoom(1, -1);
        // Display depth data by linearly mapping depth between 0 and 2 meters to the red channel
        glRasterPos2f(-1, 1);
        glDrawPixels(640, 480, GL_BGR, GL_UNSIGNED_BYTE, IPCV.getImage().data);
        //glPixelTransferf(GL_RED_SCALE, 0xFFFF * dev->get_depth_scale() / 2.0f);
        //glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, rs.getDepthImage().data);
        //glPixelTransferf(GL_RED_SCALE, 1.0f);
        // Display color image as RGB triples
        //glRasterPos2f(0, 1);
        //glDrawPixels(640, 480, GL_BGR, GL_UNSIGNED_BYTE, IPCV.getImage().data);
        if(IPCV.getIDs().size()>0 && IPCV.getIDs()[0]==228)
            {
                for(int i = 0;i<IPCV.getIDs().size();i++)
                {
                    cv::Mat oneRvecs(3,1,CV_64FC1);
                    cv::Mat rotMat(4, 4, CV_64F);
                    cv::Mat oneTvecs(3,1,CV_64FC1);
                    Vec3d tvec = IPCV.getTvec(i);
                    //cout << "0:" <<tvec[0] << "1:" <<tvec[1] << "2:" <<tvec[2] << endl;
                    for (int a = 0;a<3;a++)
                    {
                        oneRvecs.row(a).col(0) = IPCV.getRvec(i)[a];
                        oneTvecs.at<double>(0,0) = tvec[0];
                        oneTvecs.at<double>(1,0) = tvec[1];
                        oneTvecs.at<double>(2,0) = tvec[2];
                    }
                    Rodrigues(oneRvecs, rotMat);
                    renderer.LoadTexture();
                    renderer.renderMesh(rotMat,oneTvecs);
                    
                    //renderer.LoadCube();
                }
            }
        glfwSwapBuffers(win);
        //glfwSetCursorPosCallback(win, cursor_position_callback);
    }
    rs.stop();
    return EXIT_SUCCESS;
}

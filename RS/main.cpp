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

glfwObject renderer = glfwObject();
int main(int argc, char * argv[])
{
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
        glClear(GL_COLOR_BUFFER_BIT);
        glPixelZoom(1, -1);
        // Display depth data by linearly mapping depth between 0 and 2 meters to the red channel
        glRasterPos2f(-1, 1);
        //glPixelTransferf(GL_RED_SCALE, 0xFFFF * dev->get_depth_scale() / 2.0f);
        glDrawPixels(640, 480, GL_BGR, GL_UNSIGNED_BYTE, IPCV.getImage().data);
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
                    for (int a = 0;a<3;a++)
                    {
                        oneRvecs.row(a).col(0) = IPCV.getRvec(i)[a];
                        oneTvecs = IPCV.getTvec(i);
                    }
                    Rodrigues(oneRvecs, rotMat);
                    renderer.renderMesh(rotMat,oneTvecs);
                    //renderer.LoadTexture();
                    //renderer.LoadCube();
                }
            }
        glfwSwapBuffers(win);
        //glfwSetCursorPosCallback(win, cursor_position_callback);
    }
    rs.stop();
    return EXIT_SUCCESS;
}

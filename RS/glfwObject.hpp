//
//  glfwObject.hpp
//  RS
//
//  Created by Tom Cruise on 2017/3/8.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//

#ifndef glfwObject_hpp
#define glfwObject_hpp

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <glm.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <opencv/highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "tdogl/Program.h"
#include "tdogl/Texture.h"
#include "glfwObject.hpp"
#include "ObjectLoader.hpp"

using namespace std;
class glfwObject
{
public:
    glfwObject();
    glfwObject(string,string); //load .Ojb & texture(ppm)
    void setRotationX(float);
    void setRotationY(float);
    void mouseMoveHanding(double,double);
    float getRotationX();
    float getRotationY();
    void renderMesh(cv::Mat,cv::Mat);
    void glfwDrawTorus(int,int,float,float);
    void drawObj(GLMmodel*);
    void LoadCube();
    void LoadTexture();
private:
    void setObjectModel(string);
    void setObjectTexture(string);
    double RotationX;
    double RotationY;
    double RecordedX;
    double RecordedY;
    tdogl::Texture* gTexture = NULL;
    tdogl::Program* gProgram = NULL;
};
#endif /* glfwObject_hpp */

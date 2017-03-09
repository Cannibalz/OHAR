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
#include <GLFW/glfw3.h>
#include <iostream>
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
    void renderMesh();
    void glfwDrawTorus(int,int,float,float);
    
    private:
    void setObjectModel(string);
    void setObjectTexture(string);
    double RotationX;
    double RotationY;
    double RecordedX;
    double RecordedY;
};
#endif /* glfwObject_hpp */

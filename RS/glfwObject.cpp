//
//  glfwObject.cpp
//  RS
//
//  Created by Tom Cruise on 2017/3/8.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glfwObject.hpp"
glfwObject::glfwObject()
{
    glfwObject::RotationX = 0;
    glfwObject::RotationY = 0;
}
glfwObject::glfwObject(string objFileName,string textureFileName)
{
    
}
float glfwObject::getRotationX()
{
    return glfwObject::RotationX;
}
float glfwObject::getRotationY()
{
    return glfwObject::RotationY;
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
void glfwObject::renderMesh()
{
    glPushMatrix();
    glLoadIdentity();//移動中心
    glRotatef(glfwObject::getRotationX()/*+(float)record_x*/, 0.0, 1.0, 0.0);//以y軸當旋轉軸
    glRotatef(glfwObject::getRotationY()/*+(float)record_y*/, 1.0, 0.0, 0.0);//以x軸當旋轉軸
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
    
    glfwObject::glfwDrawTorus(10, 10, 0.5, .2);
    glPopMatrix();
}
void glfwObject::mouseMoveHanding(double xpos, double ypos)
{
    glfwObject::RotationX = glfwObject::RecordedX - xpos;
    glfwObject::RotationY = glfwObject::RecordedY - ypos;
}

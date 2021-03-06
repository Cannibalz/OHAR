//
//  realsenseController.hpp
//  RS
//
//  Created by Tom Cruise on 2017/3/30.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//

#ifndef realsenseController_hpp
#define realsenseController_hpp

#include <stdio.h>
#include <cstdio>
#include <librealsense/rs.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;
#endif /* realsenseController_hpp */
class realSense
{
public:
    realSense();
    Mat getColorImage();
    Mat getDepthImage();
    Mat getAlignedD2C();
    Mat getAlignedC2D();
    void waitForNextFrame();
    void stop();
private:
    rs::context ctx;
    rs::device *dev;
};

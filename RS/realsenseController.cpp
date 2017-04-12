//
//  realsenseController.cpp
//  RS
//
//  Created by Tom Cruise on 2017/3/30.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//

#include "realsenseController.hpp"
using namespace std;
realSense::realSense() try
{
    printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
    if(ctx.get_device_count() == 0)
    {ㄒ
        printf("no device connected");
    }
    
    // This tutorial will access only a single device, but it is trivial to extend to multiple devices
    dev = ctx.get_device(0); //裝置
    dev->enable_stream(rs::stream::depth, rs::preset::best_quality);
    dev->enable_stream(rs::stream::color, rs::preset::best_quality);
    printf("\nUsing device 0, an %s\n", dev->get_name());
    printf("Serial number: %s\n", dev->get_serial());
    printf("Firmware version: %s\n", dev->get_firmware_version());
    // Configure all streams to run at VGA resolution at 60 frames per second
    dev->enable_stream(rs::stream::depth, 640, 480, rs::format::z16, 30);
    dev->enable_stream(rs::stream::color, 640, 480, rs::format::bgr8, 30);
    dev->enable_stream(rs::stream::infrared, 640, 480, rs::format::y8, 30);
    try { dev->enable_stream(rs::stream::infrared2, 640, 480, rs::format::y8, 30); }
    catch(...) { printf("Device does not provide infrared2 stream.\n"); }
    dev->start(); //start streaming
}
catch(const rs::error & e) //Realsense例外：裝置被佔用 無法讀取 ..etc
{
    // Method calls against librealsense objects may throw exceptions of type rs::error
    printf("rs::error was thrown when calling %s(%s):\n", e.get_failed_function().c_str(), e.get_failed_args().c_str());
    printf("    %s\n", e.what());
    //return EXIT_FAILURE;
}
void realSense::waitForNextFrame()
{
    dev->wait_for_frames(); //取得下一幀
}
cv::Mat realSense::getColorImage()
{
    cv::Mat color(cv::Size(640, 480), CV_8UC3, (void*)dev->get_frame_data(rs::stream::color), cv::Mat::AUTO_STEP);
    Mat returnColor;
    color.copyTo(returnColor);
    return returnColor;
}
cv::Mat realSense::getDepthImage()
{
    rs::intrinsics depth_intr = dev->get_stream_intrinsics(rs::stream::depth);
    cv::Mat depth16( depth_intr.height,depth_intr.width,CV_16U,(void*)dev->get_frame_data(rs::stream::depth) );
    cv::Mat depth8u = depth16;
    depth8u.convertTo( depth8u, CV_8UC1, 255.0/10000 );
    //uint16_t *depthImage = (uint16_t *) dev->get_frame_data(rs::stream::depth);
    Mat returnDepth;//(depth_intr.height,depth_intr.width,CV_16UC1,depthImage);
    depth8u.copyTo(returnDepth);
    return returnDepth;
}
Mat realSense::getAlignedC2D()
{
    cv::Mat alignedC2D(cv::Size(640,480),CV_8UC3,(void*)dev->get_frame_data(rs::stream::color_aligned_to_depth), cv::Mat::AUTO_STEP);
    uchar* pCad = (uchar*)dev->get_frame_data(rs::stream::color_aligned_to_depth);
    Mat returnC2D(480,640,CV_8UC3,pCad);
    //alignedC2D.copyTo(returnC2D);
    return returnC2D;
}
Mat realSense::getAlignedD2C()
{
    cv::Mat returnD2C;
    return returnD2C;
}
void realSense::stop()
{
    dev->stop();
}

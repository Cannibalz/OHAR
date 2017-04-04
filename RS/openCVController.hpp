//
//  openCVController.hpp
//  RS
//
//  Created by Tom Cruise on 2017/3/30.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//

#ifndef openCVController_hpp
#define openCVController_hpp

#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#endif /* openCVController_hpp */
using namespace cv;
using namespace std;
class ipcv
{
public:
    ipcv();
    ipcv(cv::Mat);
    void DetectAndDrawMarkers();
    void RefreshFrame(cv::Mat);
    vector<int> getIDs();
    Mat getImage();
    Vec3d getRvec(int);
    Vec3d getTvec(int);
    Mat SobelEdgeDetect(Mat);
private:
    Mat ARImage;
    Mat inputImage;
    Mat cameraMatrix, distCoeffs; //相機參數
    float markerLength;
    vector< Vec3d > rvecs, tvecs;
    vector<int>ids;
    vector<vector<Point2f>>corners, rejected;
    Ptr<cv::aruco::Dictionary> dictionary;
};

//
//  openCVController.cpp
//  RS
//
//  Created by Tom Cruise on 2017/3/30.
//  Copyright © 2017年 Tom Cruise. All rights reserved.
//

#include "openCVController.hpp"

ipcv::ipcv()
{
    markerLength = 0.05f;
    dictionary = cv::aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL);
    cv::String filename = "/Users/TomCruise/Desktop/CameraParas.yml";   //Pro
    //cv::String filename = "/Users/TomCruise/Desktop/CameraParas.yml";   //iMac
    cv::FileStorage fs;
    fs.open(filename, cv::FileStorage::READ);
    fs["Camera_Matrix"] >> cameraMatrix;
    fs["Distortion_Coefficients"] >> distCoeffs;
}
ipcv::ipcv(cv::Mat Image)
{
    ipcv();
    inputImage = Image;
    ARImage = Image;
}
void ipcv::RefreshFrame(cv::Mat Image)
{
    inputImage = Image;
}
void ipcv::DetectAndDrawMarkers()
{
    cv::aruco::detectMarkers(inputImage, dictionary, corners, ids);
    inputImage.copyTo(ARImage);
    if(ids.size()>0)
    {
        cv::Mat oneRvecs(3,1,CV_64FC1);
        cv::Mat rotMat(4, 4, CV_64F);
        cv::Mat oneTvecs(3,1,CV_64FC1);
        aruco::drawDetectedMarkers(ARImage, corners, ids);
        float markerLength = 0.05;
        aruco::estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);
        for (int a = 0;a<3;a++)
        {
            oneRvecs.row(a).col(0) = rvecs[0][a];
            oneTvecs = tvecs[0];
            //cout << oneTvecs.at<double>(0,0) << "," << oneTvecs.at<double>(0,1) << "," << oneTvecs.at<double>(0,2);
        }
        Rodrigues(oneRvecs, rotMat);
        
        for(int j = 0;j<ids.size();j++)
        {
            aruco::drawAxis(ARImage, cameraMatrix, distCoeffs, rvecs[j], tvecs[j], 0.1);
        }
    }
}
vector<int> ipcv::getIDs()
{
    return ids;
}
Vec3d ipcv::getRvec(int index)
{
    return rvecs[index];
}
Vec3d ipcv::getTvec(int index)
{
    return tvecs[index];
}
Mat ipcv::getImage()
{
    return ARImage;
}

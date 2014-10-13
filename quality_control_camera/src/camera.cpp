/*
 * camera.cpp
 *
 *  Created on: Oct 10, 2014
 *      Author: Frederik Hegger
 */

#include "camera.h"

Camera::Camera() : video_capture_(NULL), is_connected_(false)
{
}

Camera::~Camera()
{
    if(video_capture_ == NULL)
        delete video_capture_;
}

bool Camera::connect(const unsigned int &device_id)
{
    video_capture_ = new cv::VideoCapture(device_id);

    if(video_capture_->isOpened())
    {
        is_connected_ = true;
        return true;
    }

    return false;
}

bool Camera::is_connected()
{
    if(is_connected_ && video_capture_->isOpened())
        return true;

    return false;
}

bool Camera::getImage(cv::Mat &image)
{
    cv::Mat captured_image;

    if(!video_capture_->read(image))
    {
        is_connected_ = false;
        return false;
    }

    is_connected_ = true;
    return true;
}

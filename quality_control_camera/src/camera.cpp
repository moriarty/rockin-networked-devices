/*
 * camera.cpp
 *
 *  Created on: Oct 10, 2014
 *      Author: Frederik Hegger
 */

#include "camera.h"

Camera::Camera() :
        video_capture_(NULL)
{
    video_capture_ = new cv::VideoCapture();
}

Camera::~Camera()
{
    video_capture_->release();

    if (video_capture_ != NULL)
        delete video_capture_;
}

bool Camera::connect(const unsigned int &device_id)
{
    if (!video_capture_->open(device_id))
        return false;

    if (!isConnected())
    {
        video_capture_->release();
        return false;
    }

    return true;
}

void Camera::disconnect()
{
    video_capture_->release();
}

bool Camera::isConnected()
{
    if (video_capture_->isOpened() && video_capture_->grab())
        return true;

    return false;
}

bool Camera::getImage(cv::Mat &image)
{
    cv::Mat captured_image;

    if (!video_capture_->read(image))
        return false;

    return true;
}

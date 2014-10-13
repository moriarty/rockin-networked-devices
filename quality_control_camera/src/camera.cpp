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

    is_connected_   = true;
    return true;
}

bool Camera::setResolution(const unsigned int &width, const unsigned int &height)
{
    double current_width = 0;
    /*
    current_width = video_capture_->get(CV_CAP_PROP_FRAME_WIDTH);

    if(current_width == 0)
        return false;

    if(!video_capture_->set(CV_CAP_PROP_FRAME_WIDTH, width))
        return false;
    if(!video_capture_->set(CV_CAP_PROP_FRAME_HEIGHT, height))
    {
        // if height could not be set, reset the width
        video_capture_->set(CV_CAP_PROP_FRAME_WIDTH, current_width);
        return false;
    }
    */
    return true;
}

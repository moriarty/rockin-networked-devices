/*
 * camera.h
 *
 *  Created on: Oct 10, 2014
 *      Author: Frederik Hegger
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <opencv2/opencv.hpp>

class Camera
{
    public:
        Camera();
        virtual ~Camera();

        bool connect(const unsigned int &device_id);
        bool is_connected();

        bool getImage(cv::Mat &image);
        bool setResolution(const unsigned int &width, const unsigned int &height);

    private:
        cv::VideoCapture *video_capture_;
        bool is_connected_;
};

#endif /* CAMERA_H_ */

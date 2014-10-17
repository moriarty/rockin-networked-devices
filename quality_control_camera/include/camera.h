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
        void disconnect();
        bool isConnected();

        bool getImage(cv::Mat &image);

    private:
        cv::VideoCapture *video_capture_;
};

#endif /* CAMERA_H_ */

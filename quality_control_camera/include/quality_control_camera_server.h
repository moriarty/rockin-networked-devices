/*
 * qualitity_control_camera_server.h
 *
 *  Created on: Oct 10, 2014
 *      Author: Frederik Hegger
 */

#ifndef QUALITY_CONTROL_CAMERA_SERVER_H_
#define QUALITY_CONTROL_CAMERA_SERVER_H_

#include <boost/lexical_cast.hpp>
#include <zmq.hpp>

#include "camera.h"

// Protobuf message headers
#include <Image.pb.h>
#include <Camera.pb.h>


class QualityControlCameraServer
{
    public:
        QualityControlCameraServer();
        virtual ~QualityControlCameraServer();

        /**
         * starts a ZMQ publisher (sending status messages)
         *
         * @param ip_address ip address or interface name of the device on which the publisher sends the data
         * @param status_msg_port port on which the server sends status messages
         */
        void startPublisher(const std::string &ip_address, const unsigned int &status_msg_port);

        /**
         * starts a ZMQ service (request/reply)
         *
         * @param ip_address ip address or interface name of the device on which the service is provided
         * @param service_port port port on which the service is advertised
         */
        void startService(const std::string &ip_address, const unsigned int &service_port);

        /**
         * checks and processes incoming data
         */
        bool checkAndProcessRequests();

        /**
         * sends a status message
         */
        void sendStatusMessage();

    private:
        void packImageIntoMessage(const cv::Mat &image, Image &img_msg);

        unsigned int default_camera_device_id_;
        Camera *camera_;

        zmq::context_t *zmq_context_;

        zmq::socket_t *zmq_publisher_;
        zmq::socket_t *zmq_service_;
};

#endif /* QUALITY_CONTROL_CAMERA_SERVER_H_ */

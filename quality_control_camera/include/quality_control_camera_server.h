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
#include <DeviceCompressedImage.pb.h>
#include <DeviceCamera.pb.h>

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
         * @return true if the publisher established successful, otherwise false
         */

        bool startPublisher(const std::string &ip_address, const unsigned int &status_msg_port);

        /**
         * destroys the publisher socket
         */
        void stopPublisher();

        /**
         * starts a ZMQ service (request/reply)
         *
         * @param ip_address ip address or interface name of the device on which the service is provided
         * @param service_port port port on which the service is advertised
         * @return true if the publisher established successful, otherwise false
         */
        bool startService(const std::string &ip_address, const unsigned int &service_port);

        /**
         * destroys the service socket
         */
        void stopService();

        /**
         *
         * @return true if the ZMQ communication is initialized, otherwise false
         */
        bool isCommunctionInitialized();

        /**
         * ToDo: docu
         * @return
         */
        bool connectCamera();

        /**
         * ToDo: doco
         */
        void disconnectCamera();

        /**
         * ToDo: docu
         * @return
         */
        bool isCameraConnected();

        /**
         * checks and processes incoming data
         *
         * @retval 0 request received, processed and replied successfully
         * @retval -1 no request available
         * @retval -2 communication error
         * @retval -3 wrong message type received
         * @retval -4 could not retrieve an image from the camera
         */
        int checkAndProcessRequests();

        /**
         * sends a status message
         * @return true if the message was sent successful, otherwise false
         */
        bool sendStatusMessage();

        /**
         * ToDo: docu
         *
         * @param is_connected
         * @return
         */
        bool sendStatusMessage(bool is_connected);

    private:
        void packImageIntoMessage(const cv::Mat &image, CompressedImage &img_msg);

        /**
         * sends an empty image as a reply to a request
         *
         * @return true if the message was sent successful, otherwise false
         */
        bool sendEmptyImage();

        unsigned int default_camera_device_id_;
        Camera *camera_;

        zmq::context_t *zmq_context_;
        zmq::socket_t *zmq_publisher_;
        zmq::socket_t *zmq_service_;
        bool is_zmq_communication_initalized_;
};

#endif /* QUALITY_CONTROL_CAMERA_SERVER_H_ */

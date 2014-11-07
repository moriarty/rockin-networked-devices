/*
 * conveyor_belt_server.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#include <quality_control_camera_server.h>

QualityControlCameraServer::QualityControlCameraServer() :
        zmq_context_(NULL), zmq_publisher_(NULL), zmq_service_(NULL), default_camera_device_id_(0), is_zmq_communication_initalized_(false)
{
    // create ZMQ context
    zmq_context_ = new zmq::context_t(1);

    camera_ = new Camera();
}

QualityControlCameraServer::~QualityControlCameraServer()
{
    if (zmq_publisher_ != NULL)
        delete zmq_publisher_;

    if (zmq_service_ != NULL)
        delete zmq_service_;

    if (zmq_context_ != NULL)
        delete zmq_context_;

    if (camera_ != NULL)
        delete camera_;

    is_zmq_communication_initalized_ = false;
}

bool QualityControlCameraServer::startPublisher(const std::string &ip_address, const unsigned int &status_msg_port)
{
    uint64_t hwm = 1;

    // add publisher to send status messages
    try
    {
        zmq_publisher_ = new zmq::socket_t(*zmq_context_, ZMQ_PUB);
        zmq_publisher_->setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));

        zmq_publisher_->bind(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(status_msg_port)).c_str());
        is_zmq_communication_initalized_ = true;
    } catch (...)
    {
        is_zmq_communication_initalized_ = false;
    }

    return is_zmq_communication_initalized_;
}

void QualityControlCameraServer::stopPublisher()
{
    is_zmq_communication_initalized_ = false;
    zmq_publisher_->close();
}

bool QualityControlCameraServer::startService(const std::string &ip_address, const unsigned int &service_port)
{
    // add service
    try
    {
        zmq_service_ = new zmq::socket_t(*zmq_context_, ZMQ_REP);

        zmq_service_->bind(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(service_port)).c_str());
        is_zmq_communication_initalized_ = true;
    } catch (...)
    {
        is_zmq_communication_initalized_ = false;
    }

    return is_zmq_communication_initalized_;
}

void QualityControlCameraServer::stopService()
{
    is_zmq_communication_initalized_ = false;
    zmq_service_->close();
}

bool QualityControlCameraServer::isCommunctionInitialized()
{
    return is_zmq_communication_initalized_;
}

bool QualityControlCameraServer::connectCamera()
{
    return camera_->connect(default_camera_device_id_);
}

void QualityControlCameraServer::disconnectCamera()
{
    camera_->disconnect();
}

bool QualityControlCameraServer::isCameraConnected()
{
    return camera_->isConnected();
}

int QualityControlCameraServer::checkAndProcessRequests()
{
    cv::Mat image;
    zmq::message_t zmq_request;
    ImageRequest img_request_msg;
    Image img_msg;
    std::string serialized_string;

    if (!camera_->isConnected())
        return -4;

    // check if a new zmq_response is present
    if (!zmq_service_->recv(&zmq_request, ZMQ_NOBLOCK))
    {
        if (zmq_errno() == EAGAIN)
            return -1;
        else
            return -2;
    }

    // check if the correct message type was received
    if (!img_request_msg.ParseFromArray(zmq_request.data(), zmq_request.size()))
    {
        sendEmptyImage();
        return -3;
    }

    // check if retrieving an image from the camera was successful
    if (!camera_->getImage(image))
    {
        sendEmptyImage();
        return -4;
    }

    packImageIntoMessage(image, img_msg);

    img_msg.SerializeToString(&serialized_string);
    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());

    if (zmq_service_->send(*reply) != 0 && (zmq_errno() != EAGAIN))
    {
        delete reply;
        return -2;
    }

    delete reply;

    return 0;
}

void QualityControlCameraServer::packImageIntoMessage(const cv::Mat &image, Image &img_msg)
{
    img_msg.set_width(image.cols);
    img_msg.set_height(image.rows);
    img_msg.set_encoding(16);
    img_msg.set_step(image.cols * image.elemSize());
    img_msg.set_data(image.data, image.rows * image.cols * image.elemSize());
}

bool QualityControlCameraServer::sendStatusMessage()
{
    return sendStatusMessage(camera_->isConnected());
}

bool QualityControlCameraServer::sendStatusMessage(bool is_connected)
{
    CameraStatus status_msg = CameraStatus();
    std::string serialized_string;

    status_msg.set_is_device_connected(is_connected);

    status_msg.SerializeToString(&serialized_string);

    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());

    if (zmq_publisher_->send(*reply) == 0 && (zmq_errno() != EAGAIN))
    {
        delete reply;
        return false;
    }

    delete reply;
    return true;
}

bool QualityControlCameraServer::sendEmptyImage()
{
    Image img_msg;
    std::string serialized_string;

    img_msg.set_height(0);
    img_msg.set_width(0);
    img_msg.set_encoding(0);
    img_msg.set_step(0);

    const char d = ' ';
    img_msg.set_data(&d);

    img_msg.SerializeToString(&serialized_string);
    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());

    if (zmq_service_->send(*reply) == 0 && (zmq_errno() != EAGAIN))
    {
        delete reply;
        return false;
    }

    delete reply;

    return true;
}

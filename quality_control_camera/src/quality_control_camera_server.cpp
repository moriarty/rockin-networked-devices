/*
 * conveyor_belt_server.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#include <quality_control_camera_server.h>

QualityControlCameraServer::QualityControlCameraServer() :
        zmq_context_(NULL), zmq_publisher_(NULL), zmq_service_(NULL), default_camera_device_id_(0)
{
    // create ZMQ context
    zmq_context_ = new zmq::context_t(1);

    camera_ = new Camera();

    // connect to the physical camera device
    while (!camera_->connect(default_camera_device_id_))
        sleep(1);
}

QualityControlCameraServer::~QualityControlCameraServer()
{
    if (zmq_context_ != NULL)
        delete zmq_context_;

    if (zmq_publisher_ != NULL)
        delete zmq_publisher_;

    if (zmq_service_ != NULL)
        delete zmq_service_;
}

void QualityControlCameraServer::startPublisher(const std::string &ip_address, const unsigned int &status_msg_port)
{
    // add publisher to send status messages
    zmq_publisher_ = new zmq::socket_t(*zmq_context_, ZMQ_PUB);
    zmq_publisher_->bind(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(status_msg_port)).c_str());
}

void QualityControlCameraServer::startService(const std::string &ip_address, const unsigned int &service_port)
{
    // add service
    zmq_service_ = new zmq::socket_t(*zmq_context_, ZMQ_REP);
    zmq_service_->bind(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(service_port)).c_str());
}

bool QualityControlCameraServer::checkAndProcessRequests()
{
    cv::Mat image;
    zmq::message_t zmq_request;
    ImageRequest img_request_msg;
    Image img_msg;
    std::string serialized_string;

    sendStatusMessage();

    // check if a new zmq_response is present0
    if (!zmq_service_->recv(&zmq_request, ZMQ_NOBLOCK))
        return false;

    std::cout << "NEW REQUEST" << std::endl;


    // check if the correct message type was received
    if (!img_request_msg.ParseFromArray(zmq_request.data(), zmq_request.size()))
    {
        //ToDo: send error reply
        return false;
    }

    // check if retrieving a image from the camera was successful
    if (!camera_->getImage(image))
    {
        //TODO: send error reply
        img_msg.set_height(0);
        img_msg.set_width(0);
        img_msg.set_encoding(0);
        img_msg.set_step(0);

        const char d = 'c';
        img_msg.set_data(&d);

        img_msg.SerializeToString(&serialized_string);
        zmq::message_t *query = new zmq::message_t(serialized_string.length());
        memcpy(query->data(), serialized_string.c_str(), serialized_string.length());
        zmq_service_->send(*query);

        return false;
    }

    if (img_request_msg.has_resolution())
    {
        if(img_request_msg.resolution() == ImageRequest::WXGA_1280_720)
            camera_->setResolution(320, 160);
    }

    packImageIntoMessage(image, img_msg);

    img_msg.SerializeToString(&serialized_string);
    zmq::message_t *query = new zmq::message_t(serialized_string.length());
    memcpy(query->data(), serialized_string.c_str(), serialized_string.length());
    zmq_service_->send(*query);


    return true;
}

void QualityControlCameraServer::packImageIntoMessage(const cv::Mat &image, Image &img_msg)
{
    img_msg.set_width(image.cols);
    img_msg.set_height(image.rows);
    img_msg.set_encoding(16);
    img_msg.set_step(image.cols * image.elemSize());
    img_msg.set_data(image.data, image.rows * image.cols * image.elemSize());
}

void QualityControlCameraServer::sendStatusMessage()
{
    CameraStatus status_msg = CameraStatus();
    std::string serialized_string;

    status_msg.set_is_device_connected(camera_->is_connected());

    status_msg.SerializeToString(&serialized_string);

    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());
    zmq_publisher_->send(*reply);

    delete reply;
}

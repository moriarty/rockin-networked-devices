/*
 * conveyor_belt_server.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#include <conveyor_belt_server.h>

ConveyorBeltServer::ConveyorBeltServer() : zmq_context_(NULL), zmq_socket_(NULL)
{
    conveyor_device_ = new ConveyorBeltKfD44();

    while (conveyor_device_->connect("/dev/conveyor") != 0)
        sleep(1);
}

ConveyorBeltServer::~ConveyorBeltServer()
{
    if (zmq_context_ != NULL)
        delete zmq_context_;

    if (zmq_socket_ != NULL)
        delete zmq_socket_;
}

void ConveyorBeltServer::provideService(const std::string ip_address, const unsigned int port)
{
    zmq_context_ = new zmq::context_t(1);
    zmq_socket_ = new zmq::socket_t(*zmq_context_, ZMQ_REP);

    zmq_socket_->bind(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(port)).c_str());
}

void ConveyorBeltServer::update()
{
    zmq::message_t request;
    ConveyorBeltCommandMessage conveyor_command_msg = ConveyorBeltCommandMessage();
    ConveyorBeltStatusMessage conveyor_status_msg = ConveyorBeltStatusMessage();

    // Wait for next client request
    if (zmq_socket_->recv(&request) == -1)
    {
        sendErrorCode(ConveyorBeltStatusMessage::RECEIVE_FAILED);
        return;
    }

    // parse conveyor belt msgs. If it fails it was a different type of msg
    if (!conveyor_command_msg.ParseFromArray(request.data(), request.size()))
    {
        sendErrorCode(ConveyorBeltStatusMessage::WRONG_MESSAGE_TYPE);
        return;
    }

    if (!conveyor_command_msg.has_mode())
    {
        setConveyorBeltParameters(conveyor_command_msg);
        sendErrorCode(ConveyorBeltStatusMessage::OK);
    }
    else
    {
        sendStatus();
    }

}

void ConveyorBeltServer::sendStatus()
{
    ConveyorBeltStatusMessage status_msg = ConveyorBeltStatusMessage();
    std::string serialized_string;

    status_msg.set_error_code(ConveyorBeltStatusMessage::OK);
    status_msg.set_is_device_connected(conveyor_device_->is_connected());
    //status_msg.set_is_belt_moving(ToDo);

    status_msg.SerializeToString(&serialized_string);

    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());
    zmq_socket_->send(*reply);

    delete reply;
}

void ConveyorBeltServer::sendErrorCode(ConveyorBeltStatusMessage::ErrorCode error_code)
{
    ConveyorBeltStatusMessage status_msg = ConveyorBeltStatusMessage();
    std::string serialized_string;

    status_msg.set_error_code(error_code);
    status_msg.SerializeToString(&serialized_string);

    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());
    zmq_socket_->send(*reply);

    delete reply;
}

void ConveyorBeltServer::setConveyorBeltParameters(ConveyorBeltCommandMessage msg)
{
    if (msg.has_mode())
    {
        if (msg.mode() == ConveyorBeltCommandMessage::START_FORWARD)
            conveyor_device_->start(ConveyorBeltKfD44::FORWARD);
        else if (msg.mode() == ConveyorBeltCommandMessage::START_REVERSE)
            conveyor_device_->start(ConveyorBeltKfD44::REVERSE);
        else if (msg.mode() == ConveyorBeltCommandMessage::STOP)
            conveyor_device_->stop();
    }
}

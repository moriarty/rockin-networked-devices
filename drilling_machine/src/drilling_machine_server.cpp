/*
 * conveyor_belt_server.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#include "drilling_machine_server.h"

DrillingMachineServer::DrillingMachineServer() :
        zmq_context_(NULL), zmq_publisher_(NULL), zmq_subscriber_(NULL), is_zmq_communication_initalized_(false)
{
    // create ZMQ context
    zmq_context_ = new zmq::context_t(1);

    drilling_machine_ = new DrillingMachine();
}

DrillingMachineServer::~DrillingMachineServer()
{
    if (zmq_publisher_ != NULL)
        delete zmq_publisher_;

    if (zmq_subscriber_ != NULL)
        delete zmq_subscriber_;

    if (zmq_context_ != NULL)
        delete zmq_context_;

    if (drilling_machine_ != NULL)
        delete drilling_machine_;

    is_zmq_communication_initalized_ = false;
}

bool DrillingMachineServer::startPublisher(const std::string &ip_address, const unsigned int &status_msg_port)
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

void DrillingMachineServer::stopPublisher()
{
    is_zmq_communication_initalized_ = false;
    zmq_publisher_->close();
}

bool DrillingMachineServer::startSubscriber(const std::string &ip_address, const unsigned int &service_port)
{
    uint64_t hwm = 1;

    // add subscriber
    try
    {
        zmq_subscriber_ = new zmq::socket_t(*zmq_context_, ZMQ_SUB);
        zmq_subscriber_->setsockopt(ZMQ_SUBSCRIBE, "", 0);
        zmq_subscriber_->setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));

        zmq_subscriber_->connect(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(service_port)).c_str());
        is_zmq_communication_initalized_ = true;
    } catch (...)
    {
        is_zmq_communication_initalized_ = false;
    }

    return is_zmq_communication_initalized_;
}

void DrillingMachineServer::stopSubscriber()
{
    is_zmq_communication_initalized_ = false;
    zmq_subscriber_->close();
}

bool DrillingMachineServer::isCommunctionInitialized()
{
    return is_zmq_communication_initalized_;
}

bool DrillingMachineServer::connectDrillingMachine()
{
    return drilling_machine_->connect();
}

void DrillingMachineServer::disconnectDrillingMachine()
{
    drilling_machine_->disconnect();
}

bool DrillingMachineServer::isDrillingMachineConnected()
{
    return drilling_machine_->isConnected();
}

int DrillingMachineServer::checkAndProcessRequests()
{
    zmq::message_t zmq_request;
    DrillingMachineCommand machine_command;
    std::string serialized_string;

    if (!drilling_machine_->isConnected())
        return -4;

    // check if a new zmq message is present
    if (!zmq_subscriber_->recv(&zmq_request, ZMQ_NOBLOCK))
    {
        if (zmq_errno() == EAGAIN)
            return -1;
        else
            return -2;
    }

    // check if the correct message type was received
    if (!machine_command.ParseFromArray(zmq_request.data(), zmq_request.size()))
        return -3;

    if (machine_command.command() == DrillingMachineCommand::MOVE_DOWN)
        drilling_machine_->moveDrillDown();
    else if (machine_command.command() == DrillingMachineCommand::MOVE_UP)
        drilling_machine_->moveDrillUp();
    else
        return -5;

    return 0;
}

bool DrillingMachineServer::sendStatusMessage()
{
    return sendStatusMessage(drilling_machine_->isConnected());
}

bool DrillingMachineServer::sendStatusMessage(bool is_connected)
{
    DrillingMachineStatus status_msg = DrillingMachineStatus();
    std::string serialized_string;

    status_msg.set_is_device_connected(is_connected);

    if (is_connected)
    {
        if(drilling_machine_->getMotorDirection() == DrillingMachine::MOVING_DOWN)
            status_msg.set_state(DrillingMachineStatus::MOVING_DOWN);
        if(drilling_machine_->getMotorDirection() == DrillingMachine::MOVING_UP)
            status_msg.set_state(DrillingMachineStatus::MOVING_UP);
        else if(drilling_machine_->getMotorPosition() == 1)
            status_msg.set_state(DrillingMachineStatus::AT_BOTTOM);
        else if(drilling_machine_->getMotorPosition() == 2)
            status_msg.set_state(DrillingMachineStatus::AT_TOP);
        else
            status_msg.set_state(DrillingMachineStatus::UNKNOWN);
    }

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

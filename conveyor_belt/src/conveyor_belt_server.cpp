/*
 * conveyor_belt_server.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#include <conveyor_belt_server.h>

ConveyorBeltServer::ConveyorBeltServer() : zmq_context(NULL), zmq_socket(NULL)
{
    conveyor_device = new ConveyorBeltKfD44();

    while (conveyor_device->connect("/dev/conveyor") != 0)
        sleep(1);
}

ConveyorBeltServer::~ConveyorBeltServer()
{
    if (zmq_context != NULL)
        delete zmq_context;

    if (zmq_socket != NULL)
        delete zmq_socket;
}

void ConveyorBeltServer::provideService(const std::string ip_address, const unsigned int port)
{
    zmq_context = new zmq::context_t(1);
    zmq_socket = new zmq::socket_t(*zmq_context, ZMQ_REP);

    zmq_socket->bind(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(port)).c_str());
}

void ConveyorBeltServer::update()
{
    zmq::message_t request;
    ConveyorBeltMessage conveyor_msg = ConveyorBeltMessage();

    // Wait for next client request
    if (zmq_socket->recv(&request) == -1)
        sendStatus(ConveyorBeltMessage::RECEIVE_FAILED);

    // parse conveyor belt msg. If it fails it was a different type of msg
    if (!conveyor_msg.ParseFromArray(request.data(), request.size()))
        sendStatus(ConveyorBeltMessage::WRONG_MESSAGE_TYPE);

    setConveyorBeltParameters(conveyor_msg);

    // Send success
    sendStatus(ConveyorBeltMessage::SUCCESS);
}

void ConveyorBeltServer::sendStatus(ConveyorBeltMessage::StatusCode status)
{
    ConveyorBeltMessage status_msg = ConveyorBeltMessage();
    std::string serialized_string;

    status_msg.set_status_code(status);
    status_msg.SerializeToString(&serialized_string);

    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());
    zmq_socket->send(*reply);

    delete reply;
}

void ConveyorBeltServer::setConveyorBeltParameters(ConveyorBeltMessage msg)
{
    // execute actions based on which variables are set
    if (msg.has_velocity())
        conveyor_device->setVelocity(quantity<boost::units::si::velocity>(msg.velocity() * si::meter_per_second));

    if (msg.has_mode())
    {
        if (msg.mode() == ConveyorBeltMessage::START_FORWARD)
            conveyor_device->start(ConveyorBeltKfD44::FORWARD);
        else if (msg.mode() == ConveyorBeltMessage::START_REVERSE)
            conveyor_device->start(ConveyorBeltKfD44::REVERSE);
        else if (msg.mode() == ConveyorBeltMessage::STOP)
            conveyor_device->stop();
    }
}

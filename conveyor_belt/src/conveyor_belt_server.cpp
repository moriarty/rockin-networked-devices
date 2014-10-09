/*
 * conveyor_belt_server.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#include <conveyor_belt_server.h>

ConveyorBeltServer::ConveyorBeltServer() :
        zmq_context_(NULL), zmq_publisher_(NULL), zmq_subscriber_(NULL)
{
    zmq_context_ = new zmq::context_t(1);
    conveyor_device_ = new ConveyorBeltKfD44();

    while (conveyor_device_->connect("/dev/conveyor") != 0)
        sleep(1);
}

ConveyorBeltServer::~ConveyorBeltServer()
{
    if (zmq_context_ != NULL)
        delete zmq_context_;

    if (zmq_publisher_ != NULL)
        delete zmq_publisher_;

    if (zmq_subscriber_ != NULL)
        delete zmq_subscriber_;
}

void ConveyorBeltServer::startPublisher(const std::string ip_address, const unsigned int status_msg_port)
{
    // add publisher to send status messages
    zmq_publisher_ = new zmq::socket_t(*zmq_context_, ZMQ_PUB);
    zmq_publisher_->bind(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(status_msg_port)).c_str());
}

void ConveyorBeltServer::startSubscriber(const std::string ip_address, const unsigned int command_msg_port)
{
    // add subscriber to receive command messages from a client
    zmq_subscriber_ = new zmq::socket_t(*zmq_context_, ZMQ_SUB);
    zmq_subscriber_->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    zmq_subscriber_->connect(std::string("tcp://" + ip_address + ":" + boost::lexical_cast<std::string>(command_msg_port)).c_str());
}

void ConveyorBeltServer::receiveAndProcessData()
{
    zmq::message_t zmq_message;
    ConveyorBeltCommandMessage conveyor_command_msg = ConveyorBeltCommandMessage();

    // check if a new a new message has arrived and if it is a conveyor belt command message
    if (zmq_subscriber_->recv(&zmq_message, ZMQ_NOBLOCK) && conveyor_command_msg.ParseFromArray(zmq_message.data(), zmq_message.size()))
        setConveyorBeltParameters(conveyor_command_msg);

}

void ConveyorBeltServer::sendStatusMessage()
{
    ConveyorBeltStatusMessage status_msg = ConveyorBeltStatusMessage();
    std::string serialized_string;

    status_msg.set_is_device_connected(conveyor_device_->is_connected());

    if(conveyor_device_->getRunState() == ConveyorBeltKfD44::STARTED)
        status_msg.set_mode(START);
    else
        status_msg.set_mode(STOP);

    status_msg.SerializeToString(&serialized_string);

    zmq::message_t *reply = new zmq::message_t(serialized_string.length());
    memcpy(reply->data(), serialized_string.c_str(), serialized_string.length());
    zmq_publisher_->send(*reply);

    delete reply;
}

void ConveyorBeltServer::setConveyorBeltParameters(ConveyorBeltCommandMessage msg)
{
    quantity<si::frequency> default_frequency = 75 * si::hertz;

    if (msg.has_mode())
    {
        if (msg.mode() == START)
        {
            conveyor_device_->setFrequency(default_frequency);
            conveyor_device_->start(ConveyorBeltKfD44::FORWARD);
        }
        else if (msg.mode() == STOP)
            conveyor_device_->stop();
    }
}

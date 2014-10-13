/*
 * conveyor_belt_server.h
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#ifndef CONVEYOR_BELT_SERVER_H_
#define CONVEYOR_BELT_SERVER_H_

#include <boost/lexical_cast.hpp>
#include <conveyor_belt_kf_d44.h>
#include <iostream>
#include <string>
#include <zmq.hpp>

#include <ConveyorBelt.pb.h>

using namespace boost::units;

class ConveyorBeltServer
{
    public:
        ConveyorBeltServer();
        virtual ~ConveyorBeltServer();

        /**
         * starts a ZMQ publisher (sending status messages)
         *
         * @param ip_address ip address or interface name of the device on which the publisher sends the data
         * @param status_msg_port port on which the server sends status messages
         */
        void startPublisher(const std::string ip_address, const unsigned int status_msg_port);

        /**
         * starts a ZMQ subscriber (receiving command messages)
         *
         * @param ip_address ip address of the device to subscribe to
         * @param command_msg_port port port to connect to
         */
        void startSubscriber(const std::string ip_address, const unsigned int command_msg_port);

        /**
         * checks and processes incoming data
         */
        void receiveAndProcessData();

        /**
         * sends a status message
         */
        void sendStatusMessage();

    private:

        /**
         * Reads the parameters from the message and sets the respective parameters for the conveyor belt device
         *
         * @param msg message with the parameters to be set
         */
        void setConveyorBeltParameters(ConveyorBeltCommandMessage msg);

        zmq::context_t *zmq_context_;

        zmq::socket_t *zmq_publisher_;
        zmq::socket_t *zmq_subscriber_;

        ConveyorBeltKfD44 *conveyor_device_;
};

#endif /* CONVEYOR_BELT_SERVER_H_ */

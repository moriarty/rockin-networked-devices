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

#include <conveyor_belt.pb.h>

using namespace boost::units;

class ConveyorBeltServer
{
    public:
        ConveyorBeltServer();
        virtual ~ConveyorBeltServer();

        /**
         * starts a ZMQ publisher (sending status messages) and subscriber (receiving command messages)
         *
         * @param ip_address ip address of the device on which the service is provided
         * @param command_msg_port port on which the server listens for incoming command messages
         * @param status_msg_port port on which the server send status messages
         */
        void start(const std::string ip_address, const unsigned int command_msg_port, const unsigned int status_msg_port);

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

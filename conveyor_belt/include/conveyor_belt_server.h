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
#include <iostream>     //ToDo: to be removed
#include <string>
#include <zmq.hpp>

#include <conveyor_belt_command.pb.h>
#include <conveyor_belt_status.pb.h>

using namespace boost::units;

class ConveyorBeltServer
{
    public:
        ConveyorBeltServer();
        virtual ~ConveyorBeltServer();

        /**
         * provides a service (request/reply) on a ZMQ socket
         *
         * @param ip_address ip address of the device on which the service is provided
         * @param port port on which the service is provided
         */
        void provideService(const std::string ip_address, const unsigned int port);

        /**
         * checks and processes incoming requests and sends a reply afterwards
         */
        void update();

    private:

        /**
         * sends a status message as a reply of a request
         */
        void sendStatus();

        /**
         * sets the StatusCode field of the message and send it as a reply
         *
         * @param status status code
         */
        void sendErrorCode(ConveyorBeltStatusMessage::ErrorCode error_code);

        /**
         * Reads the parameters from the message and sets the respective parameters for the conveyor belt device
         *
         * @param msg message with the parameters to be set
         */
        void setConveyorBeltParameters(ConveyorBeltCommandMessage msg);

        zmq::context_t *zmq_context_;
        zmq::socket_t *zmq_socket_;

        ConveyorBeltKfD44 *conveyor_device_;
};

#endif /* CONVEYOR_BELT_SERVER_H_ */

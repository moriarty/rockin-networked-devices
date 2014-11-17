/*
 * drilling_machine_server.h
 *
 *  Created on: Oct 31, 2014
 *      Author: Frederik Hegger
 */

#ifndef DRILLING_MACHINE_SERVER_H_
#define DRILLING_MACHINE_SERVER_H_

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <zmq.hpp>

#include "drilling_machine.h"

// Protobuf message headers
#include <DrillingMachine.pb.h>

class DrillingMachineServer
{
    public:
        DrillingMachineServer();
        virtual ~DrillingMachineServer();

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
         * starts a ZMQ subscriber
         *
         * @param ip_address ip address or interface name of the device on which the subscriber listens
         * @param service_port port port on which the subscriber listens
         * @return true if the subscriber established successful, otherwise false
         */
        bool startSubscriber(const std::string &ip_address, const unsigned int &service_port);

        /**
         * destroys the subscriber socket
         */
        void stopSubscriber();

        /**
         *
         * @return true if the ZMQ communication is initialized, otherwise false
         */
        bool isCommunctionInitialized();

        /**
         * ToDo: docu
         * @return
         */
        bool connectDrillingMachine();

        /**
         * ToDo: doco
         */
        void disconnectDrillingMachine();

        /**
         * ToDo: docu
         * @return
         */
        bool isDrillingMachineConnected();

        /**
         * checks and processes incoming data
         *
         * @retval 0    command received and processed
         * @retval -1   no command message received
         * @retval -2   communication error
         * @retval -3   wrong message type received
         * @retval -4   no connection to drilling machine (anymore)
         * @retval -5   the sent command is not supported
         *
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

        DrillingMachine *drilling_machine_;

        zmq::context_t *zmq_context_;
        zmq::socket_t *zmq_publisher_;
        zmq::socket_t *zmq_subscriber_;
        bool is_zmq_communication_initalized_;

        boost::posix_time::ptime last_processed_msg_time_;
};

#endif /* DRILLING_MACHINE_SERVER_H_ */

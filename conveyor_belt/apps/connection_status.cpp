#include <iostream>
#include <zmq.hpp>

#ifndef ZMQ_DONTWAIT
#   define ZMQ_DONTWAIT   ZMQ_NOBLOCK
#endif
#ifndef ZMQ_RCVHWM
#   define ZMQ_RCVHWM     ZMQ_HWM
#endif
#ifndef ZMQ_SNDHWM
#   define ZMQ_SNDHWM     ZMQ_HWM
#endif
#if ZMQ_VERSION_MAJOR == 2
#   define more_t int64_t
#   define zmq_ctx_destroy(context) zmq_term(context)
#   define zmq_msg_send(msg,sock,opt) zmq_send (sock, msg, opt)
#   define zmq_msg_recv(msg,sock,opt) zmq_recv (sock, msg, opt)
#   define ZMQ_POLL_MSEC    1000        //  zmq_poll is usec
#elif ZMQ_VERSION_MAJOR >= 3
#   define more_t int
#   define ZMQ_POLL_MSEC    1           //  zmq_poll is msec
#endif

#include <DeviceConveyorBelt.pb.h>
#include <stdio.h>
#include <unistd.h>


void receiveAndPrintStatusMessage(zmq::socket_t &subscriber)
{
    zmq::message_t zmq_message;
    ConveyorBeltStatus status_msg;

    if (subscriber.recv(&zmq_message, ZMQ_NOBLOCK))
    {
        status_msg.ParseFromArray(zmq_message.data(), zmq_message.size());
        std::cout << "is the device connected: " << status_msg.is_device_connected() << std::endl;
        std::cout << "mode: " << status_msg.mode() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    zmq::context_t context(1);

    more_t hwm = 1;

    // add subscriber to receive status messages from a the conveyor belt
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    subscriber.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
    subscriber.setsockopt(ZMQ_RCVHWM, &hwm, sizeof(hwm));
    subscriber.connect("tcp://192.168.1.101:55501");

    while(true)
    {
        receiveAndPrintStatusMessage(subscriber);

        sleep(1);
    }


    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

#include <iostream>
#include <zmq.hpp>
#include <ConveyorBelt.pb.h>
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

    uint64_t hwm = 1;

    // add subscriber to receive status messages from a the conveyor belt
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    subscriber.setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));
    subscriber.connect("tcp://conveyor-belt:55501");

    while(true)
    {
        receiveAndPrintStatusMessage(subscriber);

        sleep(1);
    }


    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

#include <iostream>
#include <zmq.hpp>
#include <ConveyorBelt.pb.h>
#include <stdio.h>
#include <unistd.h>

void sleep_with_progress(unsigned int seconds)
{
    for (size_t i = 0; i < seconds; ++i)
    {
        std::cout << ". " << std::flush;
        sleep(1);
    }
    std::cout << std::endl;
}

void sendMessage(zmq::socket_t &publisher, ConveyorBeltCommand msg)
{
    std::string serialized_string;

    // send message
    msg.SerializeToString(&serialized_string);
    zmq::message_t *query = new zmq::message_t(serialized_string.length());
    memcpy(query->data(), serialized_string.c_str(), serialized_string.length());

    publisher.send(*query);

    delete query;
}

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
    ConveyorBeltCommand conveyor_command_msg;

    zmq::context_t context(1);

    uint64_t hwm = 1;

    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));
    publisher.bind("tcp://eth0:55502");

    // add subscriber to receive status messages from a the conveyor belt
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    subscriber.setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));
    subscriber.connect("tcp://conveyor-belt:55501");

    // give the publisher/subscriber some time to get ready
    sleep(1);

    receiveAndPrintStatusMessage(subscriber);

    std::cout << "Moving the belt in FORWARD direction for 5 seconds " << std::flush;
    conveyor_command_msg = ConveyorBeltCommand();
    conveyor_command_msg.set_mode(START);
    sendMessage(publisher, conveyor_command_msg);
    sleep_with_progress(5);

    receiveAndPrintStatusMessage(subscriber);

    std::cout << "Stopping the belt" << std::endl;
    conveyor_command_msg = ConveyorBeltCommand();
    conveyor_command_msg.set_mode(STOP);
    sendMessage(publisher, conveyor_command_msg);

    receiveAndPrintStatusMessage(subscriber);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

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

int main(int argc, char *argv[])
{
    ConveyorBeltCommand conveyor_command_msg;

    zmq::context_t context(1);

    more_t hwm = 1;

    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
    publisher.setsockopt(ZMQ_RCVHWM, &hwm, sizeof(hwm));
    publisher.bind("tcp://eth0:55502");

    // give the publisher some time to get ready
    sleep(1);

    std::cout << "Moving the belt in FORWARD direction for 5 seconds " << std::flush;
    conveyor_command_msg = ConveyorBeltCommand();
    conveyor_command_msg.set_mode(START);
    sendMessage(publisher, conveyor_command_msg);
    sleep_with_progress(5);

    std::cout << "Stopping the belt" << std::endl;
    conveyor_command_msg = ConveyorBeltCommand();
    conveyor_command_msg.set_mode(STOP);
    sendMessage(publisher, conveyor_command_msg);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

#include <iostream>
#include <zmq.hpp>
#include <conveyor_belt_command.pb.h>
#include <conveyor_belt_status.pb.h>
#include <stdio.h>


void sleep_with_progress(unsigned int seconds)
{
    for(size_t i = 0; i < seconds; ++i)
    {
        std::cout << ". " << std::flush;
        sleep(1);
    }
    std::cout << std::endl;
}

int sendMessage(zmq::socket_t &socket, ConveyorBeltCommandMessage msg)
{
    std::string serialized_string;
    zmq::message_t request;

    // send message
    msg.SerializeToString(&serialized_string);
    zmq::message_t *query = new zmq::message_t(serialized_string.length());
    memcpy(query->data(), serialized_string.c_str(), serialized_string.length());
    socket.send(*query);

    // wait for reply
    ConveyorBeltStatusMessage status_msg;
    socket.recv(&request);
    status_msg.ParseFromArray(request.data(), request.size());
    return status_msg.error_code();
}


int main(int argc, char *argv[])
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    ConveyorBeltCommandMessage conveyor_msg;

    socket.connect("tcp://10.20.121.46:5555");


    std::cout << "Moving the belt in FORWARD direction for 5 seconds " << std::flush;
    conveyor_msg = ConveyorBeltCommandMessage();
    conveyor_msg.set_mode(ConveyorBeltCommandMessage::START_FORWARD);
    sendMessage(socket, conveyor_msg);
    sleep_with_progress(5);

    std::cout << "Moving the belt in REVERSE direction for 5 seconds " << std::flush;
    conveyor_msg = ConveyorBeltCommandMessage();
    conveyor_msg.set_mode(ConveyorBeltCommandMessage::START_REVERSE);
    sendMessage(socket, conveyor_msg);
    sleep_with_progress(5);

    std::cout << "Stopping the belt" << std::endl;
    conveyor_msg = ConveyorBeltCommandMessage();
    conveyor_msg.set_mode(ConveyorBeltCommandMessage::STOP);
    sendMessage(socket, conveyor_msg);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

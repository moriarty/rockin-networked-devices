#include <iostream>
#include <zmq.hpp>
#include <conveyor_belt.pb.h>
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

int sendMessage(zmq::socket_t &socket, ConveyorBeltMessage msg)
{
    std::string serialized_string;
    zmq::message_t request;

    // send message
    msg.SerializeToString(&serialized_string);
    zmq::message_t *query = new zmq::message_t(serialized_string.length());
    memcpy(query->data(), serialized_string.c_str(), serialized_string.length());
    socket.send(*query);

    // wait for reply
    socket.recv(&request);
    msg.ParseFromArray(request.data(), request.size());
    return msg.status_code();
}


int main(int argc, char *argv[])
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    ConveyorBeltMessage conveyor_msg;

    socket.connect("tcp://127.0.0.1:5555");


    std::cout << "Set maximum belt velocity (75Hz)" << std::endl;
    conveyor_msg = ConveyorBeltMessage();
    conveyor_msg.set_velocity(7500);
    sendMessage(socket, conveyor_msg);

    std::cout << "Moving the belt in FORWARD direction for 5 seconds " << std::flush;
    conveyor_msg = ConveyorBeltMessage();
    conveyor_msg.set_mode(ConveyorBeltMessage::START_FORWARD);
    sendMessage(socket, conveyor_msg);
    sleep_with_progress(5);

    std::cout << "Set belt velocity to (30Hz) and continue moving for 5 seconds" << std::flush;
    conveyor_msg = ConveyorBeltMessage();
    conveyor_msg.set_velocity(3000);
    sendMessage(socket, conveyor_msg);
    sleep_with_progress(5);

    std::cout << "Moving the belt in REVERSE direction for 5 seconds " << std::flush;
    conveyor_msg = ConveyorBeltMessage();
    conveyor_msg.set_mode(ConveyorBeltMessage::START_REVERSE);
    sendMessage(socket, conveyor_msg);
    sleep_with_progress(5);

    std::cout << "Stopping the belt" << std::endl;
    conveyor_msg = ConveyorBeltMessage();
    conveyor_msg.set_mode(ConveyorBeltMessage::STOP);
    sendMessage(socket, conveyor_msg);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

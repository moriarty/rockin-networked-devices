#include <iostream>
#include <zmq.hpp>
#include <stdio.h>
#include <unistd.h>

#include <DrillingMachine.pb.h>

void sleep_with_progress(unsigned int seconds)
{
    for (size_t i = 0; i < seconds; ++i)
    {
        std::cout << ". " << std::flush;
        sleep(1);
    }
    std::cout << std::endl;
}

void sendMessage(zmq::socket_t &publisher, DrillingMachineCommand msg)
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
    DrillingMachineCommand command_msg;

    zmq::context_t context(1);

    uint64_t hwm = 1;

    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));
    publisher.bind("tcp://eth0:55512");

    // give the publisher some time to get ready
    sleep(1);

    std::cout << "Moving drilling machine down " << std::flush;
    command_msg = DrillingMachineCommand();
    command_msg.set_command(DrillingMachineCommand::MOVE_DOWN);
    sendMessage(publisher, command_msg);
    sleep_with_progress(10);

    std::cout << "Moving drilling machine up " << std::flush;
    command_msg = DrillingMachineCommand();
    command_msg.set_command(DrillingMachineCommand::MOVE_UP);
    sendMessage(publisher, command_msg);
    sleep_with_progress(10);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

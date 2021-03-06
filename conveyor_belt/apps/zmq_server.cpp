#include <boost/asio.hpp>

#include "conveyor_belt_server.h"

using namespace boost::posix_time;
using namespace boost::asio;

enum State
{
    INIT_COMMUNICATION, CONNECT_TO_DEVICE, UPDATE
};

State connectToDevice(State &component_state, ConveyorBeltServer &server)
{
    server.sendStatusMessage();

    if (!server.connectConveyorBelt("/dev/conveyor"))
    {
        sleep(1);   // try to connect only every second in case of an error
        return CONNECT_TO_DEVICE;
    }

    return UPDATE;
}

State initializeCommunication(State &component_state, ConveyorBeltServer &server)
{
    if (server.isCommunctionInitialized())
    {
        server.stopPublisher();
        server.stopSubscriber();
    }

    if (!server.startPublisher("eth0", 55501))
    {
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    if (!server.startSubscriber("192.168.1.100", 55502))
    {
        server.stopPublisher();
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    return CONNECT_TO_DEVICE;
}

State update(State &component_state, ConveyorBeltServer &server)
{
    server.receiveAndProcessData();
    server.sendStatusMessage();

    return UPDATE;
}

int main(int argc, char *argv[])
{
    State component_state = INIT_COMMUNICATION;
    ConveyorBeltServer server = ConveyorBeltServer();

    io_service io_srv;

    while (true)
    {
        // the main loop runs with 20Hz (i.e. check for incoming data)
        deadline_timer loop_timer(io_srv, milliseconds(50));

        switch (component_state)
        {
            case INIT_COMMUNICATION:
                component_state = initializeCommunication(component_state, server);
            break;
            case CONNECT_TO_DEVICE:
                component_state = connectToDevice(component_state, server);
            break;
            case UPDATE:
                component_state = update(component_state, server);
            break;
        }

        loop_timer.wait();
    }

    return 0;
}

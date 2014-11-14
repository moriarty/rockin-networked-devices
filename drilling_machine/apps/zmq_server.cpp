#include <boost/asio.hpp>

#include "drilling_machine_server.h"

using namespace boost::posix_time;
using namespace boost::asio;

enum State
{
    INIT_COMMUNICATION, CONNECT_TO_DEVICE, UPDATE
};

State initializeCommunication(State &component_state, DrillingMachineServer &server)
{
    if (server.isCommunctionInitialized())
    {
        server.stopPublisher();
        server.stopSubscriber();
    }

    if (!server.startPublisher("eth0", 55511))
    {
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    if (!server.startSubscriber("192.168.1.100", 55512))
    {
        server.stopPublisher();
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    return CONNECT_TO_DEVICE;
}

State connectToDevice(State &component_state, DrillingMachineServer &server)
{
    server.sendStatusMessage();

    if (server.isDrillingMachineConnected())
        server.disconnectDrillingMachine();

    if (!server.connectDrillingMachine())
    {
        sleep(1);   // try only every second to connect to the device
        return CONNECT_TO_DEVICE;
    }

    return UPDATE;
}

State update(State &component_state, DrillingMachineServer &server)
{
    int process_result = -2;
    bool status_result = -2;

    process_result = server.checkAndProcessRequests();
    status_result = server.sendStatusMessage();

    if (process_result == -2 || !status_result)    // communication error
        return INIT_COMMUNICATION;
    else if (process_result == -4)   // device error
        return CONNECT_TO_DEVICE;

    return UPDATE;
}

int main(int argc, char *argv[])
{
    State component_state = INIT_COMMUNICATION;
    DrillingMachineServer server = DrillingMachineServer();

    io_service io_srv;

    while (true)
    {
        // the main loop runs every 20ms (i.e. check for incoming data)
        deadline_timer loop_timer(io_srv, milliseconds(20));

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

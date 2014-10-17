#include <conveyor_belt_server.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;
using namespace boost::asio;

enum State
{
    INIT_COMMUNICATION, CONNECT_TO_DEVICE, UPDATE
};

State connectToDevice(State &component_state, ConveyorBeltServer &server)
{
    if(!server.connectConveyorBelt("/dev/conveyor"))
    {
        sleep(1);   // try to connect only every second in case of an error
        return CONNECT_TO_DEVICE;
    }

    return INIT_COMMUNICATION;
}

State initializeCommunication(State &component_state, ConveyorBeltServer &server)
{
    if (server.isCommunctionInitialized())
    {
        server.stopPublisher();
        server.stopSubscriber();
    }

    if (!server.startPublisher("eth0", 55556))
    {
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    if (!server.startSubscriber("ref-box", 55555))
    {
        server.stopPublisher();
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    return UPDATE;
}

State update(State &component_state, ConveyorBeltServer &server)
{
    server.receiveAndProcessData();
    server.sendStatusMessage();

    return UPDATE;
}

int main(int argc, char *argv[])
{
    State component_state = CONNECT_TO_DEVICE;
    ConveyorBeltServer server = ConveyorBeltServer();

    time_duration time_diff;
    ptime prev_time_send = microsec_clock::local_time();
    io_service io_srv;

    while (true)
    {
        // the main loop runs every 20ms (i.e. check for incoming data)
        deadline_timer loop_timer(io_srv, milliseconds(20));

        switch (component_state)
        {
            case CONNECT_TO_DEVICE:
                std::cout << "   state ---> CONNECT_TO_DEVICE" << std::endl;
                component_state = connectToDevice(component_state, server);
            break;
            case INIT_COMMUNICATION:
                std::cout << "   state ---> INIT_COMMUNICATION" << std::endl;
                component_state = initializeCommunication(component_state, server);
            break;

            case UPDATE:
                std::cout << "   state ---> UPDATE" << std::endl;
                component_state = update(component_state, server);
            break;
        }

        loop_timer.wait();
    }

    return 0;
}

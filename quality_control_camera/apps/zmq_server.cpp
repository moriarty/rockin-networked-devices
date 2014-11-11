#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "quality_control_camera_server.h"

using namespace boost::posix_time;
using namespace boost::asio;

enum State
{
    INIT_COMMUNICATION, CONNECT_TO_DEVICE, UPDATE
};

State initializeCommunication(State &component_state, QualityControlCameraServer &server)
{
    if (server.isCommunctionInitialized())
    {
        server.stopPublisher();
        server.stopService();
    }

    if (!server.startPublisher("eth0", 55521))
    {
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    if (!server.startService("eth0", 55522))
    {
        server.stopPublisher();
        sleep(1);   // try only every second to initialize the connection
        return INIT_COMMUNICATION;
    }

    return CONNECT_TO_DEVICE;
}

State connectToDevice(State &component_state, QualityControlCameraServer &server)
{
    server.sendStatusMessage();

    if (server.isCameraConnected())
        server.disconnectCamera();

    if (!server.connectCamera())
    {
        sleep(1);   // try only every second to connect to the device
        return CONNECT_TO_DEVICE;
    }

    return UPDATE;
}

State update(State &component_state, QualityControlCameraServer &server)
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
    QualityControlCameraServer server = QualityControlCameraServer();

    time_duration time_diff;
    ptime prev_time_send = microsec_clock::local_time();
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

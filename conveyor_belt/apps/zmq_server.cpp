#include <conveyor_belt_server.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;
using namespace boost::asio;

int main(int argc, char *argv[])
{
    ConveyorBeltServer *server = new ConveyorBeltServer();

    server->start("127.0.0.1", 55555, 55556);

    time_duration time_diff;
    ptime prev_time_send = microsec_clock::local_time();
    io_service io_srv;

    while(true)
    {
        // the main loop runs every 20ms (i.e. check for incoming data)
        deadline_timer loop_timer(io_srv, milliseconds(20));

        server->receiveAndProcessData();

        // every 200ms a status message will be sent
        time_diff = microsec_clock::local_time() - prev_time_send;
        if(time_diff.total_milliseconds() > 200)
        {
            server->sendStatusMessage();
            prev_time_send = microsec_clock::local_time();
        }

        loop_timer.wait();
    }

    delete server;

    return 0;
}

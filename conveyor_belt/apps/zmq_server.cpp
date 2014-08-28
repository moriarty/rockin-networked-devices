#include <conveyor_belt_server.h>


int main(int argc, char *argv[])
{
    ConveyorBeltServer *server = new ConveyorBeltServer();

    server->provideService("127.0.0.1", 5555);

    while(true)
    {
        server->update();
    }

    delete server;

    return 0;
}

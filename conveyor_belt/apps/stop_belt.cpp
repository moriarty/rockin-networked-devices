#include <stdio.h>
#include <iostream>
#include <conveyor_belt_kf_d44.h>

void sleep_with_progress(unsigned int seconds)
{
    for (size_t i = 0; i < seconds; ++i)
    {
        std::cout << ". " << std::flush;
        sleep(1);
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    // name of the system device
    const std::string device_name = "/dev/conveyor";

    ConveyorBeltKfD44 *belt = new ConveyorBeltKfD44();

    std::cout << "Connecting to \"" << device_name << "\" ..." << std::endl;
    int connect = belt->connect(device_name);
    if (connect != 0)
    {
        std::cerr << "... could not connect to the device! " << std::endl;
        return -1;
    }

/*    int result;
    ConveyorBeltKfD44::RunMode run_state;

    do {
        std::cout << "Stopping the belt " << std::flush;
        result = belt->stop();
        sleep_with_progress(3);
        std::cout << result << std::endl;
        run_state = belt->getRunState();
    } while (run_state == 1);
*/    
    delete belt;

    return 0;
}

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
    int result;
    ConveyorBeltKfD44::RunMode run_state;

    ConveyorBeltKfD44 *belt = new ConveyorBeltKfD44();

    std::cout << "Connecting to \"" << device_name << "\" ..." << std::endl;
    int connect = belt->connect(device_name);
    if (connect != 0)
    {
        std::cerr << "... could not connect to the device! " << std::endl;
        std::cerr << "Belt Connect returned " <<  connect << std::endl;
        return -1;
    }

    std::cout << "Set maximum belt velocity (75Hz)" << std::endl;
    belt->setFrequency(boost::units::quantity<boost::units::si::frequency>(75 * boost::units::si::hertz));

    std::cout << "Moving the belt in FORWARD direction for 5 seconds " << std::flush;
    result = 1;

    run_state = belt->getRunState();
    while (run_state == 0) {
    	result = belt->start(ConveyorBeltKfD44::FORWARD);
        sleep_with_progress(5);
        run_state = belt->getRunState();
        std::cerr << "run state" << run_state << std::endl;
    }

    std::cout << "Set belt velocity to (30Hz) and continue moving for 5 seconds" << std::flush;
    result = 1;

    
    do {
        result = belt->setFrequency(boost::units::quantity<boost::units::si::frequency>(30 * boost::units::si::hertz));
        sleep_with_progress(5);
        run_state = belt->getRunState();
        std::cout << result << std::endl;
    } while (run_state == 0);

    std::cout << "Moving the belt in REVERSE direction for 5 seconds " << std::flush;
    do {
        result = belt->start(ConveyorBeltKfD44::REVERSE);
        sleep_with_progress(5);
    } while (result != 0);    

    std::cout << "Stopping the belt" << std::endl;
    do {
        result = belt->stop();
        run_state = belt->getRunState();
    } while (run_state == 1);


    delete belt;

    return 0;
}

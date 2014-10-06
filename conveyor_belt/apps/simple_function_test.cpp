#include <stdio.h>
#include <conveyor_belt_kf_d44.h>

void sleep_with_progress(unsigned int seconds)
{
    for(size_t i = 0; i < seconds; ++i)
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
    if (belt->connect(device_name) != 0)
    {
        std::cerr << "... could not connect to the device! " << std::endl;
        return -1;
    }

    std::cout << "Set maximum belt velocity (75Hz)" << std::endl;
    belt->setFrequency(boost::units::quantity<boost::units::si::frequency>(75 * boost::units::si::hertz));

    std::cout << "Moving the belt in FORWARD direction for 5 seconds " << std::flush;
    belt->start(ConveyorBeltKfD44::FORWARD);
    sleep_with_progress(5);

    std::cout << "Set belt velocity to (30Hz) and continue moving for 5 seconds" << std::flush;
    belt->setFrequency(boost::units::quantity<boost::units::si::frequency>(30 * boost::units::si::hertz));
    sleep_with_progress(5);

    std::cout << "Moving the belt in REVERSE direction for 5 seconds " << std::flush;
    belt->start(ConveyorBeltKfD44::REVERSE);
    sleep_with_progress(5);

    std::cout << "Stopping the belt" << std::endl;
    belt->stop();

    delete belt;

    return 0;
}

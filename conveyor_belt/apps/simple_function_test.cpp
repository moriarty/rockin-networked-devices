#include <stdio.h>
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
    if (belt->connect(device_name) != 0)
    {
        std::cerr << "... could not connect to the device! " << std::endl;
        return -1;
    }

    std::cout << "Set maximum belt velocity (75Hz)" << std::endl;
    belt->setFrequency(boost::units::quantity<boost::units::si::frequency>(75 * boost::units::si::hertz));
/**
    std::cout << "Moving the belt in FORWARD direction for 5 seconds " << std::flush;
    std::cout << "####################" << std::endl;
    belt->start(ConveyorBeltKfD44::FORWARD);
    for( int i = 0; i < 5; i++){
	std::cout << "RUN STATE: " << belt->getRunState() << std::endl;
    	sleep_with_progress(1);
    }	
    
    std::cout << "Set belt velocity to (30Hz) and continue moving for 5 seconds" << std::flush;
    belt->setFrequency(boost::units::quantity<boost::units::si::frequency>(30 * boost::units::si::hertz));
    std::cout << "####################" << std::endl;
    for( int i = 0; i < 5; i++){
        std::cout << "RUN STATE: " << belt->getRunState() << std::endl;
        sleep_with_progress(1);
    }
    //sleep_with_progress(5);


    std::cout << "Moving the belt in REVERSE direction for 5 seconds " << std::flush;
    belt->start(ConveyorBeltKfD44::REVERSE);
    std::cout << "####################" << std::endl;
    for( int i = 0; i < 5; i++){
        std::cout << "RUN STATE: " << belt->getRunState() << std::endl;
        sleep_with_progress(1);
    }
    //sleep_with_progress(5);

    std::cout << "Stopping the belt" << std::endl;
    belt->stop();
    std::cout << "####################" << std::endl;
    for( int i = 0; i < 5; i++){
        std::cout << "RUN STATE: " << belt->getRunState() << std::endl;
        sleep_with_progress(1);
    }
    
**/
    std::cout << "Stopping the belt" << std::endl;
    belt->stop();
    std::cout << "####################" << std::endl;

    //ConveyorBeltKfD44::RunMode curr_mode;
    //ConveyorBeltKfD44::RunMode new_mode;
    //int curr_mode;
    //int new_mode;

    const int total_count = 1000;
    int error_count = 0;
    int start_error = 0;
    int stop_error = 0;

    for( int i = 0; i < total_count; i++){
        std::cout << "####################" << std::endl;

        if (i%2 == 0){
	    //curr_mode = belt->getRunState();
	    std::cout << "RUN STATE: " << belt->getRunState() << std::endl;
            
            std::cout << "Stopping the belt" << std::endl;
            belt->stop();

            sleep_with_progress(1);
	    
	    //new_mode = belt->getRunState();
            std::cout << "RUN STATE: " << belt->getRunState() << std::endl;
        
	
    
        } else {
	    //curr_mode = belt->getRunState();
            std::cout << "RUN STATE: " << belt->getRunState() << std::endl;

            std::cout << "Starting the belt" << std::endl;
            belt->start(ConveyorBeltKfD44::REVERSE);

            sleep_with_progress(1);

	    //new_mode = belt->getRunState();
            std::cout << "RUN STATE: " << belt->getRunState() << std::endl;
        /*
	if (curr_mode == new_mode){
            std::cout << "!!!! ERROR !!!!" << std::endl;
            error_count++;
            start_error++;
        }
	*/

	}

    }
    
    std::cout << "\nSUMMARY\nRUNS:\t" << total_count << "\nERRORS:\t" << error_count << std::endl;

    


    std::cout << "Stopping the belt" << std::endl;
    belt->stop();
    std::cout << "####################" << std::endl;
    delete belt;

    return 0;
}

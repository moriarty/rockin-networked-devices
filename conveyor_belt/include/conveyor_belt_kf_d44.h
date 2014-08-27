/**
 * \class ConveyorBeltKfD44
 *
 * \brief Interface for the conveyor belt KF-D44 with Yaskawa V1000 frequency converter
 *
 * \author Frederik Hegger
 *
 */

#ifndef CONVEYOR_BELT_KF_D44_H_
#define CONVEYOR_BELT_KF_D44_H_

#include <bitset>
#include <boost/units/io.hpp>
#include <boost/units/systems/si/velocity.hpp>
#include <modbus/modbus.h>

#define WAIT_TIME_WRITE_PARAMETERS_IN_MS        200     // in milliseconds; Parameter H5-11 is set to "1" (see p. 408 of Yaskawa V100 documentation)
#define WAIT_TIME_CONTROL_COMMAND_IN_MS         5       // in milliseconds
#define WAIT_TIME_READ_PARAMETERS_IN_MS         5       // in milliseconds

using namespace boost::units;

class ConveyorBeltKfD44
{
    public:
        enum Direction
        {
            FORWARD = 0, REVERSE = 1
        };

        ConveyorBeltKfD44();
        virtual ~ConveyorBeltKfD44();

        /**
         *	Establishes the connection to a conveyor belt device.
         *
         * @param device_name the name of the serial interface device as it appears in the system (e.g. "/dev/ttyUSB0")
         * @retval 0 connection has been successfully established
         * @retval -1 could not create new RTU context
         * @retval -2 could not set slave ID
         * @retval -3 could not establish connection to the specified slave
         * @retval -4 "Drive Ready" bit could not be read
         */
        int connect(const std::string device_name);

        /**
         * Closes the current connection to the device.
         */
        void disconnect();

        /**
         * Get information whether a connection is to the device is established or not.
         *
         * @retval true a connection is established
         * @retval false no connection
         */
        bool is_connected();

        /**
         * Start the belt movement with a given direction
         *
         * @param direction the direction in which the belt should start to move. Either FORWARD or REVERSE
         * @retval 0 the start command has been written to the respective Modbus register
         * @retval -1 unsupported direction specified. Only FORWARD and REVERSE are supported.
         * @retval -2 the start command could not be written to the respective Modbus register
         */
        int start(Direction direction);

        /**
         * Stop the belt movement.
         *
         * @retval true the stop command has been written to the respective Modbus register
         * @retval false the stop command could not be written to the respective Modbus register
         */
        bool stop();

        /**
         * Set the velocity of the belt. The velocity can be changed in any state.
         *
         * @param desired_belt_velocity	desired belt velocity in meter per seconds (Minimum velocity = 0.01 m/sec, maximum velocity = 0.03 m/sec). ToDo
         * @retval true the velocity was set successfully
         * @retval false the velocity could not be set
         */
        bool setVelocity(const quantity<si::velocity> desired_belt_velocity);

        /**
         * Allows to enable and disable the Modbus debug messages.
         *
         * @param debug_mode_on if set to TRUE, additional debug messages from the Modbus library will be printed.
         */
        void setModbusDebugMode(const bool debug_mode_on);

    private:

        /**
         * Sets default values for various parameters
         */
        void setDefaultParameters();

        /**
         * Converts a velocity into a frequency
         *
         * @param velocity the velocity of the belt given in meters per second
         * @return a frequency in Hz * 100 (e.g. 7000 = 70Hz)
         */
        unsigned int convertVelocityToFrequency(const quantity<si::velocity> velocity);

        modbus_t *modbus_rtu_contex;

        //ToDO: read these parameter from a config file
        int slave_id;
        int baudrate;
        int data_bits;
        int stop_bits;
        char partiy;
        quantity<si::velocity> default_belt_velocity;
};

#endif /* CONVEYOR_BELT_KF_D44_H_ */

/*
 * conveyor_belt_kf_d44.cpp
 *
 *  Created on: Aug 22, 2014
 *      Author: Frederik Hegger
 */

#include "conveyor_belt_kf_d44.h"

ConveyorBeltKfD44::ConveyorBeltKfD44() :
        modbus_rtu_contex_(NULL), run_state(STOPPED)
{
    //ToDO: read these parameters from a config file
    slave_id_ = 0x1F;
    baudrate_ = 9600;
    data_bits_ = 8;
    stop_bits_ = 1;
    partiy_ = 'N';                   // no parity

    default_frequency_ = 75 * si::hertz;
}

ConveyorBeltKfD44::~ConveyorBeltKfD44()
{
    disconnect();
}

int ConveyorBeltKfD44::connect(const std::string device_name)
{
    // disconnect an old connection (if one exists) before
    disconnect();

    // configure the serial connection
    modbus_rtu_contex_ = modbus_new_rtu(device_name.c_str(), baudrate_, partiy_, data_bits_, stop_bits_);

    if (modbus_rtu_contex_ == NULL)
        return -1;

    // set slave id to connect to
    if (modbus_set_slave(modbus_rtu_contex_, slave_id_) != 0)
        return -2;

    // connect to slave device
    if (modbus_connect(modbus_rtu_contex_) != 0)
        return -3;

    if (!is_connected())
    {
        modbus_close(modbus_rtu_contex_);

        if (modbus_rtu_contex_ != NULL)
        {
            modbus_free(modbus_rtu_contex_);
            modbus_rtu_contex_ = NULL;
        }

        return -4;
    }

    setDefaultParameters();

    return 0;
}

void ConveyorBeltKfD44::disconnect()
{
    if (is_connected())
    {
        stop();
        modbus_close(modbus_rtu_contex_);
    }

    if (modbus_rtu_contex_ != NULL)
    {
        modbus_free(modbus_rtu_contex_);
        modbus_rtu_contex_ = NULL;
    }
}

bool ConveyorBeltKfD44::is_connected()
{
    if (modbus_rtu_contex_ == NULL)
        return false;

    // check if status register can be read
    uint16_t current_register_value[1];
    if (modbus_read_registers(modbus_rtu_contex_, 0x0001, 1, current_register_value) == 1)
    {
        usleep((WAIT_TIME_READ_PARAMETERS_IN_MS * 1000));
        return true;
    }
    usleep((WAIT_TIME_READ_PARAMETERS_IN_MS * 1000));

    return false;
}

int ConveyorBeltKfD44::start(Direction motor_direction)
{
    std::bitset<16> register_value(0);

    // set the direction bit
    if (motor_direction == FORWARD)
        register_value.set(1, 0);
    else if (motor_direction == REVERSE)
        register_value.set(1, 1);
    else
        return -1;

    // set run mode to start
    register_value.set(0, true);

    // write the values to the register
    if (modbus_write_register(modbus_rtu_contex_, 0x0001, (int) (register_value.to_ulong())) == 1)
    {
        usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));
        run_state = STARTED;
        return 0;
    }
    usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));

    return -2;
}

bool ConveyorBeltKfD44::stop()
{
    // write the values to the register
    if (modbus_write_register(modbus_rtu_contex_, 0x0001, 0x00) == 1)
    {
        usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));
        run_state = STOPPED;
        return true;
    }
    usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));

    return false;
}

bool ConveyorBeltKfD44::setFrequency(const quantity<si::frequency> desired_frequency)
{
    int transformed_frequency = static_cast<int>((desired_frequency.value() * 100)) ;

    // write the frequency in to the respective register
    if (modbus_write_register(modbus_rtu_contex_, 0x0002, transformed_frequency) == 1)
    {
        usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));
        return true;
    }
    usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));

    return false;
}

void ConveyorBeltKfD44::setModbusDebugMode(bool debug_mode_on)
{
    if (!is_connected())
        return;

    modbus_set_debug(modbus_rtu_contex_, debug_mode_on);
}

void ConveyorBeltKfD44::setDefaultParameters()
{
    stop();
    setFrequency(default_frequency_);
}

ConveyorBeltKfD44::RunMode ConveyorBeltKfD44::getRunState()
{
    return run_state;
}

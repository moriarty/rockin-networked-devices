/*
 * conveyor_belt_kf_d44.cpp
 *
 *  Created on: Aug 22, 2014
 *      Author: Frederik Hegger
 */

#include "conveyor_belt_kf_d44.h"

ConveyorBeltKfD44::ConveyorBeltKfD44()
{
    modbus_rtu_contex = NULL;

    //ToDO: read these parameters from a config file
    slave_id = 0x1F;
    baudrate = 9600;
    data_bits = 8;
    stop_bits = 1;
    partiy = 'N';                   // no parity

    default_belt_velocity = 7500 * si::meter_per_second;
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
    modbus_rtu_contex = modbus_new_rtu(device_name.c_str(), baudrate, partiy, data_bits, stop_bits);

    if (modbus_rtu_contex == NULL)
        return -1;

    // set slave id to connect to
    if (modbus_set_slave(modbus_rtu_contex, slave_id) != 0)
        return -2;

    // connect to slave device
    if (modbus_connect(modbus_rtu_contex) != 0)
        return -3;

    if (!is_connected())
    {
        modbus_close(modbus_rtu_contex);

        if (modbus_rtu_contex != NULL)
        {
            modbus_free(modbus_rtu_contex);
            modbus_rtu_contex = NULL;
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
        modbus_close(modbus_rtu_contex);
    }

    if (modbus_rtu_contex != NULL)
    {
        modbus_free(modbus_rtu_contex);
        modbus_rtu_contex = NULL;
    }
}

bool ConveyorBeltKfD44::is_connected()
{
    if (modbus_rtu_contex == NULL)
        return false;

    // check if status register can be read
    uint16_t current_register_value[1];
    if (modbus_read_registers(modbus_rtu_contex, 0x0001, 1, current_register_value) == 1)
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

    if (motor_direction != FORWARD && motor_direction != REVERSE)
        return -1;

    // set the direction bit
    register_value.set(1, motor_direction);

    // set run mode to start
    register_value.set(0, true);

    // write the values to the register
    if (modbus_write_register(modbus_rtu_contex, 0x0001, (int) (register_value.to_ulong())) == 1)
    {
        usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));
        return 0;
    }
    usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));

    return -2;
}

bool ConveyorBeltKfD44::stop()
{
    // write the values to the register
    if (modbus_write_register(modbus_rtu_contex, 0x0001, 0x00) == 1)
    {
        usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));
        return true;
    }
    usleep((WAIT_TIME_WRITE_PARAMETERS_IN_MS * 1000));

    return false;
}

bool ConveyorBeltKfD44::setVelocity(const quantity<si::velocity> desired_belt_velocity)
{
    int velocity_as_frequency = 0;

    velocity_as_frequency = convertVelocityToFrequency(desired_belt_velocity);

    // write the frequency in to the respective register
    if(modbus_write_register(modbus_rtu_contex, 0x0002, velocity_as_frequency) == 1)
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

    modbus_set_debug(modbus_rtu_contex, debug_mode_on);
}

void ConveyorBeltKfD44::setDefaultParameters()
{
    stop();
    setVelocity(default_belt_velocity);
}

unsigned int ConveyorBeltKfD44::convertVelocityToFrequency(const quantity<si::velocity> velocity)
{
    // ToDo: add the conversion from given velocity to the conveyor belts frequency here.
    // it is only a dummy function right now

    return velocity.value();
}

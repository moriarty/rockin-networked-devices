/*
 * camera.cpp
 *
 *  Created on: Oct 10, 2014
 *      Author: Frederik Hegger
 */

#include "drilling_machine.h"

DrillingMachine::DrillingMachine() :
        motor_vertical_motion_(NULL)
{
}

DrillingMachine::~DrillingMachine()
{
    disconnect();
}

bool DrillingMachine::connect()
{
    std::string project_directory = PROJECT_DIR;

    try
    {
        motor_vertical_motion_ = new youbot::YouBotGripperTHK("drilling_machine_motor", project_directory + "/config");

        motor_vertical_motion_->doJointCommutation();

    } catch (std::exception& e)
    {
        std::cout << "Could not initialize the motor(s): " << e.what();

        if (motor_vertical_motion_ != NULL)
        {
            delete motor_vertical_motion_;
            motor_vertical_motion_ = NULL;
        }

        return false;
    }

    try
    {
        motor_vertical_motion_->calibrateGripperTHK();
    } catch (std::exception& e)
    {
        std::cout << "WARNING (can be ignored): " << e.what();
    }

    switchMotorOn();

    return true;
}

bool DrillingMachine::moveDrillDown()
{
    return moveMotor(DOWN_POSITION, (6.28 / 15.0), 2.0);
}

bool DrillingMachine::moveDrillUp()
{
    return moveMotor(UP_POSITION, (6.28 / 10.0 * 2), 0.8);
}

bool DrillingMachine::moveMotor(const float &target_position, const float &max_velocity, const float abort_current)
{
    if (motor_vertical_motion_ == NULL)
        return false;

    motor_vertical_motion_->softCloseThread(target_position, max_velocity, abort_current);

    return true;
}

void DrillingMachine::disconnect()
{
    switchMotorOff();

    if (motor_vertical_motion_ != NULL)
    {
        delete motor_vertical_motion_;
        motor_vertical_motion_ = NULL;
    }
}

bool DrillingMachine::isConnected()
{
    if (motor_vertical_motion_ == NULL)
        return false;

    return youbot::EthercatMaster::getInstance().isEtherCATConnectionEstablished();
}

void DrillingMachine::switchMotorOn()
{
    digitalWrite(1, 1);     // turn GPIO on
}

void DrillingMachine::switchMotorOff()
{
    digitalWrite(1, 0);     // turn GPIO off
}

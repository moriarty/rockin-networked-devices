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

    try
    {
        motor_vertical_motion_->softCloseThread(target_position, max_velocity, abort_current);
    } catch (...)
    {
        return false;
    }

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
    bool is_connected = false;

    if (motor_vertical_motion_ == NULL)
        return false;

    try
    {
        is_connected = youbot::EthercatMaster::getInstance().isEtherCATConnectionEstablished();
    } catch (...)
    {
        return false;
    }

    return is_connected;
}

void DrillingMachine::switchMotorOn()
{
    digitalWrite(1, 1);     // turn GPIO on
}

void DrillingMachine::switchMotorOff()
{
    digitalWrite(1, 0);     // turn GPIO off
}

int DrillingMachine::getMotorPosition()
{
    std::vector<youbot::JointSensedAngle> joint_angles;

    try
    {
        motor_vertical_motion_->getJointData(joint_angles);
    } catch (...)
    {
        return -1;
    }

    if (joint_angles.size() < 1)
        return -1;

    if (fabs(joint_angles[0].angle.value() - DOWN_POSITION) < 0.1)
        return 1;
    else if (fabs(joint_angles[0].angle.value() - UP_POSITION) < 0.1)
        return 2;
    else
        return -2;
}

DrillingMachine::MotorDirection DrillingMachine::getMotorDirection()
{
    std::vector<youbot::JointSensedVelocity> joint_velocities;

    try
    {
        motor_vertical_motion_->getJointData(joint_velocities);
    } catch (...)
    {
        return NOT_MOVING;
    }

    if (joint_velocities.size() < 1)
        return NOT_MOVING;

    if (joint_velocities[0].angularVelocity.value() > 0)
        return MOVING_UP;
    else if (joint_velocities[0].angularVelocity.value() < 0)
        return MOVING_DOWN;
    else
        return NOT_MOVING;
}

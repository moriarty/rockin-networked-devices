/*
 * drilling_machine.h
 *
 *  Created on: Oct 31, 2014
 *      Author: Frederik Hegger
 */

#ifndef DRILLING_MACHINE_H_
#define DRILLING_MACHINE_H_

#include <wiringPi.h>
#include <youbot/YouBotGripperTHK.hpp>

class DrillingMachine
{
    public:
        DrillingMachine();
        virtual ~DrillingMachine();

        bool connect();
        void disconnect();
        bool isConnected();

        bool moveDrillDown();
        bool moveDrillUp();
        bool moveMotor(const float &target_position, const float &max_velocity, const float abort_current);


    private:
        void switchMotorOn();
        void switchMotorOff();

        youbot::YouBotGripperTHK *motor_vertical_motion_;
};

#endif /* DRILLING_MACHINE_H_ */

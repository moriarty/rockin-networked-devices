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

#define DOWN_POSITION   (-3.14 * 1.5)
#define UP_POSITION     (-0.1)


class DrillingMachine
{
    public:
        enum MotorDirection
        {
            MOVING_DOWN = 0, MOVING_UP = 1, NOT_MOVING = 2
        };

        DrillingMachine();
        virtual ~DrillingMachine();

        bool connect();
        void disconnect();
        bool isConnected();

        bool moveDrillDown();
        bool moveDrillUp();
        bool moveMotor(const float &target_position, const float &max_velocity, const float abort_current);

        /**
         *
         * @retval 1    if the motor is in the DOWN position
         * @retval 2    if the motor is in the UP position
         * @retval -1   joint angle information are not available
         * @retval -2   no the motor is none of the above mentioned positions
         */
        int getMotorPosition();

        MotorDirection getMotorDirection();


    private:
        void switchMotorOn();
        void switchMotorOff();

        youbot::YouBotGripperTHK *motor_vertical_motion_;
};

#endif /* DRILLING_MACHINE_H_ */

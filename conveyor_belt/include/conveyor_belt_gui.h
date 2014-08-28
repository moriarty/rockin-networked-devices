/*
 * conveyor_belt_gui.h
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#ifndef CONVEYOR_BELT_GUI_H_
#define CONVEYOR_BELT_GUI_H_

#include <conveyor_belt.pb.h>
#include <qt4/QtGui/QtGui>
#include <qt4/QtGui/QMainWindow>
#include <ui_conveyor_belt_gui.h>
#include <zmq.hpp>


class ConveyorBeltGui: public QMainWindow, public Ui::MainWindow
{
        Q_OBJECT

    public:
        ConveyorBeltGui(QMainWindow *parent = 0);
        ~ConveyorBeltGui();

    private:
        int sendMessage(ConveyorBeltMessage msg);

        void onCloseButton();
        void onStartButton();
        void onStopButton();
        void slidermouseReleaseEvent();
        void onradioButtonForward();
        void onradioButtonReverse();

        zmq::context_t *zmq_context;
        zmq::socket_t *zmq_socket;
};

#endif /* CONVEYOR_BELT_GUI_H_ */

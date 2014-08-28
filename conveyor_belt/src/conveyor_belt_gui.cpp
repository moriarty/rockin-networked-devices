/*
 * conveyor_belt_gui.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: Frederik Hegger
 */

#include "conveyor_belt_gui.h"

#include <qt4/Qt/qslider.h>
#include <qt4/QtGui/qslider.h>
#include <qt4/Qt/qradiobutton.h>
#include <qt4/QtGui/qradiobutton.h>


ConveyorBeltGui::ConveyorBeltGui(QMainWindow *parent) :
        QMainWindow(parent), zmq_context(NULL), zmq_socket(NULL)
{
    setupUi(this);

    // add min, max + current
    slider_velocity->setRange(130, 7500);
    slider_velocity->setSingleStep(10);

    radio_button_forward;   //enable
    radio_button_reverse;

    // add click handler for
    // close
    // start
    // stop

    // add onchange event for
    // slider
    // radio button

    // initialize network connection
    zmq_context = new zmq::context_t(1);
    zmq_socket = new zmq::socket_t(*zmq_context, ZMQ_REQ);
    zmq_socket->connect("tcp://127.0.0.1:5555");

}

ConveyorBeltGui::~ConveyorBeltGui()
{
    if (zmq_context == NULL)
        delete zmq_context;
    if (zmq_socket == NULL)
        delete zmq_socket;
}

int ConveyorBeltGui::sendMessage(ConveyorBeltMessage msg)
{
    std::string serialized_string;
    zmq::message_t request;

    // send message
    msg.SerializeToString(&serialized_string);
    zmq::message_t *query = new zmq::message_t(serialized_string.length());
    memcpy(query->data(), serialized_string.c_str(), serialized_string.length());
    zmq_socket->send(*query);

    // wait for reply
    zmq_socket->recv(&request);
    msg.ParseFromArray(request.data(), request.size());
    return msg.status_code();
}

void ConveyorBeltGui::onCloseButton()
{
    exit(0);
}

void ConveyorBeltGui::onStartButton()
{
    ConveyorBeltMessage conveyor_msg = ConveyorBeltMessage();

    if (radio_button_forward->isChecked())   //ToDO
        conveyor_msg.set_mode(ConveyorBeltMessage::START_FORWARD);
    else
        conveyor_msg.set_mode(ConveyorBeltMessage::START_REVERSE);

    sendMessage(conveyor_msg);
}

void ConveyorBeltGui::onStopButton()
{
    ConveyorBeltMessage conveyor_msg = ConveyorBeltMessage();

    conveyor_msg.set_mode(ConveyorBeltMessage::STOP);

    sendMessage(conveyor_msg);
}

void ConveyorBeltGui::slidermouseReleaseEvent()
{
    ConveyorBeltMessage conveyor_msg = ConveyorBeltMessage();

    //get velocity from slider

    conveyor_msg.set_velocity(6000);

    sendMessage(conveyor_msg);
}

void ConveyorBeltGui::onradioButtonForward()
{
    ConveyorBeltMessage conveyor_msg = ConveyorBeltMessage();

    conveyor_msg.set_mode(ConveyorBeltMessage::START_FORWARD);

    sendMessage(conveyor_msg);
}

void ConveyorBeltGui::onradioButtonReverse()
{
    ConveyorBeltMessage conveyor_msg = ConveyorBeltMessage();

    conveyor_msg.set_mode(ConveyorBeltMessage::START_REVERSE);

    sendMessage(conveyor_msg);
}


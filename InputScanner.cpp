/*
 * InputScanner.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: mxa5473
 */

#include "InputScanner.h"
#include "GarageDoor.h"
#include "StateMachine.h"

// scan the input
InputScanner::GarageDoor() {}
//    while(1) {
//        InputScanner keyScanner = InputScanner();
//        event_received input = keyScanner.scanInput();
//
//        // otherwise no input
//
//    }






STATE_DEFINE(GarageDoor, idle, GarageDoorData) {
	std::cout << "Input Scanner Status: IDLE" << std::endl;
	if (data->button_pushed){}
		// event received
		// send data to mailbox
		// output message
}

STATE_DEFINE(GarageDoor, event_received, GarageDoorData) {
	if (data->button_pushed) {
		BUTTON_PUSHED = true;
		IR_INTERRUPT = false;
		OVERCURRENT = false;
	}

		//If the door is in is_open or stop states and the button is pushed the door would close.
	if (position == 1) {
		is_closed = TRUE;	// set the state flag
		std::cout << "Garage Status :: CLOSED" << std::endl;
	}

		//otherwise the door would open.
	if (position == 0){
		is_closed = TRUE;	// set the state flag
	}
	std::cout << "Garage Status :: CLOSED" << std::endl;
	if (data->overcurrent) {
		BUTTON_PUSHED = false;
		IR_INTERRUPT = true;
		OVERCURRENT = false;
	} else if (data->ir_interrupt) {
		BUTTON_PUSHED = false;
		IR_INTERRUPT = false;
		OVERCURRENT = true;
	}
	std::cout << "Input Scanner Status :: EVENT RECEIVED  " << std::endl;

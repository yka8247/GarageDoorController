/*
 * InputScanner.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: mxa5473
 */
#include <iostream>
#include <unistd.h>
#include "InputScanner.h"
#include "StateMachine.h"

// scan the input
InputScanner::InputScanner() :
	StateMachine(ST_MAX_STATES)
{}
//    while(1) {
//        InputScanner keyScanner = InputScanner();
//        event_received input = keyScanner.scanInput();
//
//        // otherwise no input
//
//    }


void InputScanner::Event_Received(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (ST_EVENT_RECEIVED)			// ST_IDLE
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_EVENT_RECEIVED
	END_TRANSITION_MAP(data)
}



STATE_DEFINE(InputScanner, idle, NoEventData) {
	std::cout << "Input Scanner Status: IDLE" << std::endl;
	//while (TRUE) { // actually while true gets called when the Event_Received gets called
	//delay(500);
	//}

}

STATE_DEFINE(InputScanner, event_received, GarageDoorData) {
	std::cout << "Input Scanner Status :: EVENT RECEIVED  " << std::endl;
	if (data->button_pushed) {
		std::cout << "INPUT:: BUTTON PUSH DETECTED" << std::endl;
		//TODO: use messaging queue to pass the event data over to Garage Door
	}
	if (data->overcurrent) {
		std::cout << "INPUT:: OVERCURRENT DETECTED" << std::endl;
		//TODO: use messaging queue to pass the event data over to Garage Door
	}
	if (data->ir_interrupt) {
		std::cout << "INPUT:: IR_BEAM_TRIGGERED" << std::endl;
		//TODO: use messaging queue to pass the event data over to Garage Door
	}
	InternalEvent(ST_IDLE);

//	if (data->button_pushed) {
//		BUTTON_PUSHED = true;
//		IR_INTERRUPT = false;
//		OVERCURRENT = false;
//	}
//
//		//If the door is in is_open or stop states and the button is pushed the door would close.
//	if (position == 1) {
//		is_closed = TRUE;	// set the state flag
//		std::cout << "Garage Status :: CLOSED" << std::endl;
//	}
//
//		//otherwise the door would open.
//	if (position == 0){
//		is_closed = TRUE;	// set the state flag
//	}
//	std::cout << "Garage Status :: CLOSED" << std::endl;
//	if (data->overcurrent) {
//		BUTTON_PUSHED = false;
//		IR_INTERRUPT = true;
//		OVERCURRENT = false;
//	} else if (data->ir_interrupt) {
//		BUTTON_PUSHED = false;
//		IR_INTERRUPT = false;
//		OVERCURRENT = true;
//	}
//	std::cout << "Input Scanner Status :: EVENT RECEIVED  " << std::endl;
}

/*
 * GarageDoor.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: yxa8247
 */
#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <hw/inout.h>
#include "GarageDoor.h"
#include "Hardware.h"


GarageDoorData* BuildEvent(char inp, uintptr_t pbHandle) {
	GarageDoorData* data = new GarageDoorData();
	switch(inp) {
	case 'p':{
		data->button_pushed = TRUE;
		break;
	}
	case 'i': {
		data->ir_interrupt = TRUE;
		break;
	}
	case 'v': {
		data->overcurrent = TRUE;
		break;
	}
	case 'o': {
		data->full_open_signal = TRUE;
		break;
	}
	case 'c': {
		data->full_close_signal = TRUE;
		break;
	}
	}
	data->pbHandle = pbHandle;
	return data;
}



GarageDoor::GarageDoor() :
	StateMachine(ST_MAX_STATES),
	full_open(FALSE),
	is_operating(FALSE),
	full_close(TRUE),
	overcurrent(FALSE),
	ir_beam_enabled(FALSE),
	ir_beam_triggered(FALSE)
{}


// invoke motor to either close or open the garage door
void GarageDoor::Operate(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY(ST_MOTOR_UP)					// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY(ST_MOTOR_DOWN)					// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY(ST_UPWARD_STOP)				// ST_MOTOR_UP
		TRANSITION_MAP_ENTRY(ST_DOWNWARD_STOP)				// ST_MOTOR_DOWN
		TRANSITION_MAP_ENTRY(ST_MOTOR_UP)					// ST_UPWARD_STOP
		TRANSITION_MAP_ENTRY(ST_MOTOR_DOWN)					// ST_DOWN_STOP
	END_TRANSITION_MAP(data)
}


// Complete and transit toward next state
void GarageDoor::Complete(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY(ST_DOOR_CLOSED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY(ST_DOOR_OPEN)					// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY(ST_DOOR_OPEN)					// ST_MOTOR_UP
		TRANSITION_MAP_ENTRY(ST_DOOR_CLOSED)				// ST_MOTOR_DOWN
		TRANSITION_MAP_ENTRY(EVENT_IGNORED)					// ST_UPWARD_STOP
		TRANSITION_MAP_ENTRY(EVENT_IGNORED)					// ST_DOWN_STOP
	END_TRANSITION_MAP(data)
}


// halt immediately upon external event
void GarageDoor::Halt(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY(ST_DOOR_CLOSED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY(ST_DOOR_OPEN)					// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY(ST_UPWARD_STOP)				// ST_MOTOR_UP
		TRANSITION_MAP_ENTRY(ST_DOWNWARD_STOP)				// ST_MOTOR_DOWN
		TRANSITION_MAP_ENTRY(ST_UPWARD_STOP)				// ST_UPWARD_STOP
		TRANSITION_MAP_ENTRY(ST_DOWNWARD_STOP)				// ST_DOWNWARD_STOP
	END_TRANSITION_MAP(data)
}


STATE_DEFINE(GarageDoor, door_closed, GarageDoorData) {
	std::cout << "Garage Status :: CLOSED" << std::endl;
	// set the state flag
	full_close = TRUE;
	// set the hardware
	uintptr_t pbHandle = data->pbHandle;
	out8(pbHandle, P8);

}


STATE_DEFINE(GarageDoor, door_open, GarageDoorData) {
	std::cout << "Garage Status :: OPEN" << std::endl;
	// set the state flag
	full_open = TRUE;
	ir_beam_enabled = TRUE;
	// set pin 3 (IR Beam On) to high
	uintptr_t pbHandle = data->pbHandle;
	int output = (P3|P8);
	out8(pbHandle, output);
}


STATE_DEFINE(GarageDoor, motor_up, GarageDoorData) {
	std::cout << "Garage Status :: MOTOR UP" << std::endl;
	// set the state flag
	full_close = FALSE;
	// set pin 1 (motor up) to high along with pin 8 for active high reset
	uintptr_t pbHandle = data->pbHandle;
	out8(pbHandle, (P1|P8));
	//std::cout << "current state: " << (char)this->GetCurrentState() << std::endl;
}


STATE_DEFINE(GarageDoor, motor_down, GarageDoorData) {
	std::cout << "Garage Status :: MOTOR DOWN" << std::endl;
	// set the flag
	full_open = FALSE;
	// set pin 2 (motor down) to high along with pin 8 for active high reset
	uintptr_t pbHandle = data->pbHandle;
	out8(pbHandle, (P2|P3|P8));
}


STATE_DEFINE(GarageDoor, upward_stop, GarageDoorData) {
	std::cout << "Garage Status :: UPWARD STOP" << std::endl;
	if (data->button_pushed) {
		std::cout << "\tPAUSE :: Button Pushed" << std::endl;
	}
	if (data->overcurrent) {
		std::cout << "\t HALT :: OverCurrent Detected" << std::endl;
	}

	// set the hardware
	uintptr_t pbHandle = data->pbHandle;
	out8(pbHandle, P8);
}

STATE_DEFINE(GarageDoor, downward_stop, GarageDoorData) {
	uintptr_t pbHandle = data->pbHandle;
	if (data->button_pushed) {
		std::cout << "PAUSE :: Button Pushed" << std::endl;
	}
	if (data->overcurrent) {
		std::cout << "\t HALT :: OverCurrent Detected" << std::endl;
	}
	if (data->ir_interrupt) {
		std::cout << "\t HALT :: Infrared Beam Interruption Detected" << std::endl;
	}
	// set the hardware
	std::cout << "Garage Status :: DOWNWARD STOP" << std::endl;
	out8(pbHandle, (P3|P8));
}


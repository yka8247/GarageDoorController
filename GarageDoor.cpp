/*
 * GarageDoor.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: yxa8247
 */
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "GarageDoor.h"


GarageDoorData* eventGenerator(char inp) {
	GarageDoorData* data = new GarageDoorData();
	switch(inp) {
	case 'B':
	case 'b':{
		data->button_pushed = TRUE;
		break;
	}
	case 'I':
	case 'i': {
		data->ir_interrupt = TRUE;
		break;
	}
	case 'O':
	case 'o': {
		data->overcurrent = TRUE;
		break;
	}
	}
	return data;
}


GarageDoor::GarageDoor() :
	StateMachine(ST_MAX_STATES),
	position(1),
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


// halt immediately external event
void GarageDoor::Halt(GarageDoorData* data)
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY(ST_DOOR_CLOSED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY(ST_DOOR_OPEN)					// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY(ST_UPWARD_STOP)				// ST_MOTOR_UP
		TRANSITION_MAP_ENTRY(ST_DOWNWARD_STOP)				// ST_MOTOR_DOWN
		TRANSITION_MAP_ENTRY(ST_UPWARD_STOP)				// ST_UPWARD_STOP
		TRANSITION_MAP_ENTRY(ST_DOWNWARD_STOP)				// ST_DOWNWARD_STOP
	END_TRANSITION_MAP(NULL)
}


void* UpwardTimer(void* args) {
	for(int i=0; i< 10; i++) {
		sleep(1);
	}
	return (FALSE);
}


void* DownwardTimer() {
	for (int i=10; i>0; i--) {
		sleep(1);
	}
	return (FALSE);
}


// state machine sits here when motor is not running
STATE_DEFINE(GarageDoor, door_closed, NoEventData) {
	if (position == 1) {
		// set the state flag
		full_close = TRUE;
		std::cout << "Garage Status :: CLOSED" << std::endl;
		// set the hardware pin values appropriately
		// @TODO: Hardware Configuration code goes here
	} else {
		std::cout << "Garage Status :: ERROR" << std::endl;
		std::cout << "Current Position :: " << position << std::endl;
		InternalEvent(ST_DOWNWARD_STOP);
	}
}


STATE_DEFINE(GarageDoor, door_open, NoEventData) {
	if (position == 10) {
		// set the state flag
		full_open = TRUE;
		ir_beam_enabled = TRUE;
		std::cout << "Garage Status :: OPEN" << std::endl;
		// set the hardware pin values appropriately
		// @TODO: Hardware Configuration code goes here
	} else {
		std::cout << "Garage Status :: ERROR" << std::endl;
		std::cout << "Current Position :: " << position << std::endl;
		InternalEvent(ST_UPWARD_STOP);
	}
}


STATE_DEFINE(GarageDoor, motor_up, GarageDoorData) {
	pthread_t timerThread;
	std::cout << "Garage Status :: MOTOR UP" << std::endl;
	full_close = FALSE;
	if (data->full_open_signal) {
		InternalEvent(ST_DOOR_OPEN, data);
	}
	//@TODO: Implement Timer right here

}


STATE_DEFINE(GarageDoor, motor_down, GarageDoorData) {
	std::cout << "Garage Status :: MOTOR DOWN" << std::endl;
	full_open = FALSE;
	if (data->full_open_signal) {
		InternalEvent(ST_DOOR_CLOSED, data);
	}
	//@TODO: Implement Timer right here

}


STATE_DEFINE(GarageDoor, upward_stop, GarageDoorData) {
	std::cout << "Garage Status :: UPWARD STOP" << std::endl;

	if (data->overcurrent == TRUE) {
		overcurrent = TRUE;				// set the OC flag
		std::cout << "Flag Caught :: OVER_CURRENT" << std::endl;
	}
	if (data->ir_interrupt == TRUE) {
		ir_beam_triggered = TRUE;		// set the IR flag
		std::cout << "Flag Caught :: IR_BEAM_TRIGGERED" << std::endl;
	}
}

STATE_DEFINE(GarageDoor, downward_stop, GarageDoorData) {
	std::cout << "Garage Status :: DOWNWARD STOP" << std::endl;

	if (data->overcurrent == TRUE) {
		overcurrent = TRUE;				// set the OC flag
		std::cout << "Flag Caught :: OVER_CURRENT" << std::endl;
	}
	if (data->ir_interrupt == TRUE) {
		ir_beam_triggered = TRUE;		// set the IR flag
		std::cout << "Flag Caught :: IR_BEAM_TRIGGERED" << std::endl;
	}
}


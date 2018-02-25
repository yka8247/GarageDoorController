/*
 * GarageDoor.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: yxa8247
 */
#include <iostream>
#include <unistd.h>
#include "GarageDoor.h"

GarageDoor::GarageDoor() :
	StateMachine(ST_MAX_STATES),
	position(1),
	is_open(FALSE),
	is_operating(FALSE),
	is_closed(TRUE),
	overcurrent(FALSE),
	ir_beam_enabled(FALSE)
{}

// initialize the GarageDoor Object
void GarageDoor::Init() {
	// TODO: figure out how to initiailize with closed state
	return;
}

// invoke motor to either close or open the garage door
void GarageDoor::Operate(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (ST_OPERATING)					// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_OPERATING
		TRANSITION_MAP_ENTRY (ST_OPERATING)					// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (ST_OPERATING)					// ST_STOP
	END_TRANSITION_MAP(data)
}

// halt immediately external event
void GarageDoor::Halt(GarageDoorData* data)
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (ST_STOP)						// ST_OPERATING
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_STOP
	END_TRANSITION_MAP(NULL)
}

// state machine sits here when motor is not running
STATE_DEFINE(GarageDoor, door_closed, NoEventData) {
	if (position == 1)
		is_closed = TRUE;	// set the state flag
		std::cout << "Garage Status :: CLOSED" << std::endl;
}

STATE_DEFINE(GarageDoor, door_open, NoEventData) {
	if (position == 10)
		is_open = TRUE;						// set the state flag
		ir_beam_enabled = TRUE;				// enable ir_beam
		std::cout << "Garage Status :: OPEN" << std::endl;
}

STATE_DEFINE(GarageDoor, stop, GarageDoorData) {
	std::cout << "Garage Status :: STOP" << std::endl;
	if (data->overcurrent)
		overcurrent = TRUE;			// set the OC flag
		std::cout << "Flag Cuaght :: OVER_CURRENT" << std::endl;
	if (data->ir_interrupt)
		ir_triggered = TRUE;		// set the IR flag
		std::cout << "Flag Cuaght :: IR_BEAM_TRIGGERED" << std::endl;
}

STATE_DEFINE(GarageDoor, operating, GarageDoorData) {
	std::cout << "Garage Status :: OPERATING" << std::endl;
	is_operating = TRUE;							// set the state flag
	if (is_closed) {
		while (position < 10) {
			std::cout << "Opening.. || Current Position: " << position << std::endl;
			if (data->overcurrent) {
				std::cout << "HALT :: OVERCURRENT DETECTED" << std::endl;
				InternalEvent(ST_STOP);
				return;
			}
			if (data->ir_interrupt) {
				std::cout << "HALT :: IR_BEAM_TRIGGERED" << std::endl;
				InternalEvent(ST_STOP);
				return;
			}
			position++;
			delay(200);
		}
		std::cout << "FULL OPEN || Current Position: " << position << std::endl;
		is_operating = FALSE;						// clear the state flag
		is_closed = FALSE;							// clear the previous flag
		InternalEvent(ST_DOOR_OPEN, data);			// move on to the next state
	}
	if (is_open) {
		while (position > 1) {
			std::cout << "Closing.... || Current Position: " << position << std::endl;
			if (data->overcurrent) {
				std::cout << "HALT :: OVERCURRENT DETECTED" << std::endl;
				InternalEvent(ST_STOP);
				return;
			}
			if (data->ir_interrupt) {
				std::cout << "HALT :: IR_BEAM_TRIGGERED" << std::endl;
				InternalEvent(ST_STOP);
				return;
			}
			position--;
			delay(200);
		}
		std::cout << "FULL CLOSED || Current Position: " << position << std::endl;
		is_operating = FALSE;						// clear the state flag
		is_open = FALSE;							// clear the previous flag
		InternalEvent(ST_DOOR_CLOSED, data);		// move on to the next state
	}
}

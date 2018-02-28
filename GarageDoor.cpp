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
	was_open(FALSE),
	full_open(FALSE),
	is_operating(FALSE),
	was_closed(TRUE),
	full_closed(TRUE),
	overcurrent(FALSE),
	ir_beam_enabled(FALSE),
	ir_beam_triggered(FALSE)
{}

// invoke motor to either close or open the garage door
void GarageDoor::Operate(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (ST_OPERATING)					// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (ST_STOP)						// ST_OPERATING
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
		was_closed = TRUE;						// set the state flag
		full_closed = TRUE;
		std::cout << "Garage Status :: CLOSED" << std::endl;
}

STATE_DEFINE(GarageDoor, door_open, NoEventData) {
	if (position == 10)
		was_open = TRUE;						// set the state flag
		full_open = TRUE;
		ir_beam_enabled = TRUE;					// enable ir_beam
		std::cout << "Garage Status :: OPEN" << std::endl;
}

STATE_DEFINE(GarageDoor, stop, GarageDoorData) {
	std::cout << "Garage Status :: STOP" << std::endl;
	if (data->overcurrent)
		overcurrent = TRUE;			// set the OC flag
		std::cout << "Flag Cuaght :: OVER_CURRENT" << std::endl;
	if (data->ir_interrupt)
		ir_beam_triggered = TRUE;		// set the IR flag
		std::cout << "Flag Cuaght :: IR_BEAM_TRIGGERED" << std::endl;
}

STATE_DEFINE(GarageDoor, operating, GarageDoorData) {
	if (is_operating) {
		is_operating = FALSE;
		InternalEvent(ST_STOP, data);
	}
	std::cout << "Garage Status :: OPERATING" << std::endl;
	is_operating = TRUE;							// set the state flag
	if (was_closed) {
		while (position < 10) {
			std::cout << "Opening.. || Current Position: " << position << std::endl;
			position++;
			sleep(1);
		}
		std::cout << "FULLY OPEN || Current Position: " << position << std::endl;
		is_operating = FALSE;						// clear the state flag
		was_closed = FALSE;							// clear the previous flag
		InternalEvent(ST_DOOR_OPEN, data);			// move on to the next state
	}
	if (was_open) {
		while (position > 1) {
			std::cout << "Closing.... || Current Position: " << position << std::endl;
			position--;
			sleep(1);
		}
		std::cout << "FULLY CLOSED || Current Position: " << position << std::endl;
		is_operating = FALSE;						// clear the state flag
		was_open = FALSE;							// clear the previous flag
		InternalEvent(ST_DOOR_CLOSED, data);		// move on to the next state
	}
}

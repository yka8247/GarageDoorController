/*
 * GarageDoor.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: yxa8247
 */
#include <iostream>
#include "GarageDoor.h"
#include <unistd.h>

GarageDoor::GarageDoor() :
	StateMachine(ST_MAX_STATES),
	position(0),
	is_open(FALSE),
	is_operating(FALSE),
	is_closed(FALSE),
	overcurrent(FALSE),
	ir_beam_enabled(FALSE)
{}

// initialize the GarageDoor Object
void GarageDoor::Init() {
	// TODO: figure out how to initiailize with closed state
	return;
}

// opening door
void GarageDoor::DoorUp(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (ST_UPWARD_OPERATION)			// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (ST_UPWARD_OPERATION)			// ST_STOP
	END_TRANSITION_MAP(data)
}

// closing door
void GarageDoor::DoorDown(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (ST_DOWNWARD_OPERATION)		// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (ST_DOWNWARD_OPERATION)		// ST_STOP
	END_TRANSITION_MAP(data)
}

// halt immediately external event
void GarageDoor::Halt()
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (ST_STOP)						// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (ST_STOP)						// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_STOP
	END_TRANSITION_MAP(NULL)
}

// state machine sits here when motor is not running
STATE_DEFINE(GarageDoor, door_closed, NoEventData) {
	is_closed = TRUE;	// set the state flag
	std::cout << "Garage Status :: CLOSED" << std::endl;
}

STATE_DEFINE(GarageDoor, upward_operation, GarageDoorData) {
	std::cout << "Garage Status :: UPWARD_OPERATION" << std::endl;
	is_operating = TRUE;	// set the state flag
	while (position < 10) {
		if (data->overcurrent) {
			std::cout << "HALT :: OVERCURRENT DETECTED" << std::endl;
			InternalEvent(ST_STOP);
			break;
		}
		position++;
		delay(200);
		std::cout << "opening..." << std::endl;
	}
	if (position == 10) {
		is_operating = FALSE;			// clear the state flag
		InternalEvent(ST_DOOR_OPEN, data);	// move on to the next state
	}
}

STATE_DEFINE(GarageDoor, door_open, NoEventData) {
	is_open = TRUE;						// set the state flag
	ir_beam_enabled = TRUE;				// enable ir_beam
	std::cout << "Garage Status :: OPEN" << std::endl;
}

STATE_DEFINE(GarageDoor, downward_operation, GarageDoorData) {
	std::cout << "Garage Status :: DOWNWARD_OPERATION" << std::endl;
	is_operating = TRUE;
	while (position >= 0) {
		if (data->overcurrent) {
			std::cout << "HALT :: OVERCURRENT DETECTED" << std::endl;
			InternalEvent(ST_STOP);
			break;
		}
		position--;
		delay(200);
		std::cout << "closing..." << std::endl;
	}
	if (position == 0) {
		is_operating = FALSE;				// clear the state flag
		InternalEvent(ST_DOOR_CLOSED, data);		// move on to the next state
	}
}

STATE_DEFINE(GarageDoor, stop, NoEventData) {
	std::cout << "Garage Status :: STOP" << std::endl;
	overcurrent = TRUE;			// set the error flag
}

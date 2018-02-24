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
	is_closed(TRUE),
	overcurrent(FALSE),
	ir_beam(FALSE)
{}

// initialize the GarageDoor Object
//void GarageDoor::Init() {
//	BEGIN_TRANSITION_MAP			              			// - Current State -
//		TRANSITION_MAP_ENTRY (ST_DOOR_CLOSED)				// ST_DOOR_CLOSED
//		TRANSITION_MAP_ENTRY (ST_DOOR_CLOSED)				// ST_UPWARD_OPERATION
//		TRANSITION_MAP_ENTRY (ST_DOOR_CLOSED)				// ST_DOOR_OPEN
//		TRANSITION_MAP_ENTRY (ST_DOOR_CLOSED)				// ST_DOWNWARD_OPERATION
//		TRANSITION_MAP_ENTRY (ST_DOOR_CLOSED)				// ST_STOP
//	END_TRANSITION_MAP(NULL)
//}

// opening door
void GarageDoor::DoorUp(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (ST_UPWARD_OPERATION)			// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_STOP
	END_TRANSITION_MAP(data)
}

// closing door
void GarageDoor::DoorDown(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (ST_DOWNWARD_OPERATION)		// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_STOP
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

// Recover from the removal of overcurrent event
void GarageDoor::Recover()
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (ST_DOWNWARD_OPERATION)		// ST_STOP
	END_TRANSITION_MAP(NULL)
}

// state machine sits here when motor is not running
STATE_DEFINE(GarageDoor, door_closed, GarageDoorData) {
	std::cout << "Garage Status::CLOSED" << std::endl;
	is_closed = TRUE;	// set the state flag
	while (is_closed) {
		delay(500);		// poll every 500 ms
		if (data->button_pushed) {
			is_closed = FALSE;
		}
	}
	InternalEvent(ST_UPWARD_OPERATION);
}

STATE_DEFINE(GarageDoor, upward_operation, GarageDoorData) {
	std::cout << "Garage Status::UPWARD_OPERATION" << std::endl;
	is_operating = TRUE;	// set the state flag
	while (position < 10) {
		position++;
		delay(200);
		std::cout << "opening..." << std::endl;
	}
	if (position == 0) {
		is_operating = FALSE;			// clear the state flag
		InternalEvent(ST_DOOR_OPEN);	// move on to the next stae
	}
}

STATE_DEFINE(GarageDoor, door_open, GarageDoorData) {
	std::cout << "Garage Status::OPEN" << std::endl;
	is_open = TRUE;			// set the state flag
	ir_beam = TRUE;			// enable ir_beam
	while (is_open) {
		delay(500);			// poll every 500 ms
		if (data->button_pushed) {
			is_open = FALSE;
		}
	}
	InternalEvent(ST_DOWNWARD_OPERATION);
}

STATE_DEFINE(GarageDoor, downward_operation, GarageDoorData) {
	std::cout << "Garage Status::DOWNWARD_OPERATION" << std::endl;
	is_operating = TRUE;
	while (position >= 0) {
		position--;
		delay(200);
		std::cout << "closing..." << std::endl;
	}
	if (position == 0) {
		is_operating = FALSE;				// clear the state flag
		InternalEvent(ST_DOOR_CLOSED);		// move on to the next state
	}
}

STATE_DEFINE(GarageDoor, stop, GarageDoorData) {
	std::cout << "Garage Status::STOP" << std::endl;
	overcurrent = TRUE;			// set the error flag
	while (overcurrent) {
		delay(500);				// poll every 500 ms
		if (!data->overcurrent) {
			overcurrent = FALSE;
		}
	}
	InternalEvent(ST_DOWNWARD_OPERATION);
}

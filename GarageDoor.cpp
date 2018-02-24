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
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_STOP
	END_TRANSITION_MAP(data)
}

// closing door
void GarageDoor::DoorDown(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (ST_DOWNWARD_OPERATION)		// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_STOP
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
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_STOP
	END_TRANSITION_MAP(NULL)
}

// Recover from the removal of overcurrent event
void GarageDoor::Recover()
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_UPWARD_OPERATION
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (ST_DOWNWARD_OPERATION)		// ST_DOWNWARD_OPERATION
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_STOP
	END_TRANSITION_MAP(NULL)
}

// state machine sits here when motor is not running
STATE_DEFINE(GarageDoor, door_closed, NoEventData) {
	std::cout << "Garage Status::CLOSED" << std::endl;
	is_operating = FALSE;
	is_closed = TRUE;
	position = 0;
}

STATE_DEFINE(GarageDoor, upward_operation, GarageDoorData) {
	std::cout << "Garage Status::UPWARD_OPERATION" << std::endl;
	is_closed = FALSE;
	is_operating = TRUE;
	while (position < 10) {
		position++;
		delay(200);
		std::cout << "opening..." << std::endl;
	}

	if (position == 10) {
		InternalEvent(ST_DOOR_OPEN);
	}
}

STATE_DEFINE(GarageDoor, door_open, NoEventData) {
	std::cout << "Garage Status::OPEN" << std::endl;
	is_operating = FALSE;
	is_open = TRUE;
	ir_beam = TRUE;
}

STATE_DEFINE(GarageDoor, downward_operation, GarageDoorData) {
	std::cout << "Garage Status::DOWNWARD_OPERATION" << std::endl;
	is_open = FALSE;
	is_operating = TRUE;
	while (position >= 0) {
		position--;
		delay(200);
		std::cout << "closing..." << std::endl;
	}
	if (position == 0) {
		InternalEvent(ST_DOOR_CLOSED);
	}
}

STATE_DEFINE(GarageDoor, stop, GarageDoorData) {
	std::cout << "Garage Status::STOP" << std::endl;
	is_operating = FALSE;
	overcurrent = TRUE;
}

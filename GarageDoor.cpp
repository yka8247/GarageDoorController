/*
 * GarageDoor.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: yxa8247
 */
#include <iostream>
#include "GarageDoor.h"

GarageDoor::GarageDoor() : StateMachine(ST_MAX_STATES) {}

void GarageDoor::DoorDown(GarageDoorData* data) {
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (ST_START)						// ST_IDLE
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_STOP
		TRANSITION_MAP_ENTRY (ST_CHANGE_SPEED)				// ST_START
		TRANSITION_MAP_ENTRY (ST_CHANGE_SPEED)				// ST_CHANGE_SPEED
	END_TRANSITION_MAP(data)
}

void GarageDoor::DoorUp(GarageDoorData* data) {

}

// halt motor external event
void GarageDoor::Halt()
{
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_IDLE
		TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)				// ST_STOP
		TRANSITION_MAP_ENTRY (ST_STOP)						// ST_START
		TRANSITION_MAP_ENTRY (ST_STOP)						// ST_CHANGE_SPEED
	END_TRANSITION_MAP(NULL)
}

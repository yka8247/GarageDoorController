/*
 * GarageDoor.cpp
 *
 *  Created on: Feb 23, 2018
 *      Author: mxa5473
 */
#include <iostream>
#include <unistd.h>
#include "InputScanner.h"

InputScanner::InputScanner():
	StateMachine(ST_MAX_STATES)
	{}

// initialize the InputScanner Object
void InputScanner::Init() {
	// TODO: figure out how to initiailize with closed state
	return;
}

void InputScanner::Idle(InputScannerData* data){
	BEGIN_TRANSITION_MAP			              			// - Current State -
			TRANSITION_MAP_ENTRY (ST_OPERATING)					// ST_DOOR_CLOSED
			TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_OPERATING
			TRANSITION_MAP_ENTRY (ST_OPERATING)					// ST_DOOR_OPEN
			TRANSITION_MAP_ENTRY (ST_OPERATING)					// ST_STOP
		END_TRANSITION_MAP(data)
	}

void InputScanner::CreateEvent(InputScannerData data){
	BEGIN_TRANSITION_MAP			              			// - Current State -
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_CLOSED
		TRANSITION_MAP_ENTRY (ST_STOP)						// ST_OPERATING
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_DOOR_OPEN
		TRANSITION_MAP_ENTRY (EVENT_IGNORED)				// ST_STOP
	END_TRANSITION_MAP(NULL)
}


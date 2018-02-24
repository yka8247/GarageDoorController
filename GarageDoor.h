/*
 * GarageDoor.h
 *
 *  Created on: Feb 23, 2018
 *      Author: yxa8247
 */

#ifndef GARAGEDOOR_H_
#define GARAGEDOOR_H_
#include "StateMachine.h"

class GarageDoorData : public EventData {
public:
	BOOL button_pushed;
	BOOL IR_interrupt;
};

class GarageDoor : public StateMachine {
public:
	GarageDoor();
	void DoorUp(GarageDoorData* data);
	void DoorrDown(GarageDoorData* data);
	void Halt();
private:
	enum States {
		ST_DOOR_CLOSED,
		ST_UPWARD_OPERATION,
		ST_DOOR_OPEN,
		ST_DOWNARD_OPERATION,
		ST_MAX_STATES
	};

	// Define the state machine state functions with event data type
	STATE_DECLARE(GarageDoor,	DOOR_CLOSED,		NoEventData)
	STATE_DECLARE(GarageDoor,	UPWARD_OPERATION,	GarageDoorData)
	STATE_DECLARE(GarageDoor,	DOOR_OPEN,			NoEventData)
	STATE_DECLARE(GarageDoor,	UPWARD_OPERATION,	GarageDoorData)

	/* State map to define state object order
	 * Each state amp entry defines a state object */
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&DoorClosed)
		STATE_MAP_ENTRY(&UpwardOperation)
		STATE_MAP_ENTRY(&DoorOpen)
		STATE_MAP_ENTRY(&DownwardOperation)
	END_STATE_MAP
};

#endif /* GARAGEDOOR_H_ */

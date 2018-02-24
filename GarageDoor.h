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
	BOOL overcurrent;
};

class GarageDoor : public StateMachine {
public:
	GarageDoor();
	void Init();
	void DoorUp(GarageDoorData* data);
	void DoorDown(GarageDoorData* data);
	void Halt();
private:
	INT position;
	BOOL is_open;
	BOOL is_operating;
	BOOL is_closed;
	BOOL overcurrent;
	BOOL ir_beam_enabled;
	BOOL ir_triggered;
	enum States {
		ST_DOOR_CLOSED,
		ST_UPWARD_OPERATION,
		ST_DOOR_OPEN,
		ST_DOWNWARD_OPERATION,
		ST_STOP,
		ST_MAX_STATES
	};

	// Define the state machine state functions with event data type
	STATE_DECLARE(GarageDoor,	door_closed,		GarageDoorData)
	STATE_DECLARE(GarageDoor,	upward_operation,	NoEventData)
	STATE_DECLARE(GarageDoor,	door_open,			GarageDoorData)
	STATE_DECLARE(GarageDoor,	downward_operation,	NoEventData)
	STATE_DECLARE(GarageDoor,	stop, 				GarageDoorData)

	/* State map to define state object order
	 * Each state amp entry defines a state object */
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&door_closed)
		STATE_MAP_ENTRY(&upward_operation)
		STATE_MAP_ENTRY(&door_open)
		STATE_MAP_ENTRY(&downward_operation)
		STATE_MAP_ENTRY(&stop)
	END_STATE_MAP
};

#endif /* GARAGEDOOR_H_ */

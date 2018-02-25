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
	BOOL ir_interrupt;
	BOOL overcurrent;
};

class GarageDoor : public StateMachine {
public:
	GarageDoor();
	void Init();
	void Operate(GarageDoorData* data);
	void Halt(GarageDoorData* data);
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
		ST_OPERATING,
		ST_DOOR_OPEN,
		ST_STOP,
		ST_MAX_STATES
	};

	// Define the state machine state functions with event data type
	STATE_DECLARE(GarageDoor,	door_closed,		NoEventData)
	STATE_DECLARE(GarageDoor,	operating,			GarageDoorData)
	STATE_DECLARE(GarageDoor,	door_open,			NoEventData)
	STATE_DECLARE(GarageDoor,	stop, 				GarageDoorData)

	/* State map to define state object order
	 * Each state amp entry defines a state object */
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&door_closed)
		STATE_MAP_ENTRY(&operating)
		STATE_MAP_ENTRY(&door_open)
		STATE_MAP_ENTRY(&stop)
	END_STATE_MAP
};

#endif /* GARAGEDOOR_H_ */

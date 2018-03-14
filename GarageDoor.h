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
	BOOL full_close_signal;
	BOOL full_open_signal;
};

GarageDoorData* KeyboardEventGenerator(char);
GarageDoorData* HardwareEventGenerator();


class GarageDoor : public StateMachine {
public:
	GarageDoor();
	void Operate(GarageDoorData* data);
	void Halt(GarageDoorData* data);

private:
	BOOL full_open;
	BOOL is_operating;
	BOOL full_close;
	BOOL overcurrent;
	BOOL ir_beam_enabled;
	BOOL ir_beam_triggered;
	enum States {
		ST_DOOR_CLOSED,
		ST_DOOR_OPEN,
		ST_MOTOR_UP,
		ST_MOTOR_DOWN,
		ST_UPWARD_STOP,
		ST_DOWNWARD_STOP,
		ST_MAX_STATES
	};

	// Define the state machine state functions with event data type
	STATE_DECLARE(GarageDoor,	door_closed,		NoEventData)
	STATE_DECLARE(GarageDoor,	door_open,			NoEventData)
	STATE_DECLARE(GarageDoor,	motor_up, 			GarageDoorData)
	STATE_DECLARE(GarageDoor,	motor_down, 		GarageDoorData)
	STATE_DECLARE(GarageDoor,	upward_stop, 		GarageDoorData)
	STATE_DECLARE(GarageDoor,	downward_stop, 		GarageDoorData)

	/* State map to define state object order
	 * Each state amp entry defines a state object */
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&door_closed)
		STATE_MAP_ENTRY(&door_open)
		STATE_MAP_ENTRY(&motor_up)
		STATE_MAP_ENTRY(&motor_down)
		STATE_MAP_ENTRY(&upward_stop)
		STATE_MAP_ENTRY(&downward_stop)
	END_STATE_MAP
};

#endif /* GARAGEDOOR_H_ */

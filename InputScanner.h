/*
 * GarageDoor.h
 *
 *  Created on: Feb 23, 2018
 *      Author: mxa5473
 */

#ifndef InputScanner_H_
#define InputScanner_H_
#include "InputScanner.h"
#include "StateMachine.h"
#include "GarageDoor.h"

class InputScanner : public StateMachine {
public:
    InputScanner();
    void Event_Received(GarageDoorData* data);
private:
    enum States {
        ST_IDLE,
        ST_EVENT_RECEIVED,
        ST_MAX_STATES
    };

    // Define the state machine state functions with event data type
    STATE_DECLARE(InputScanner,	idle,					GarageDoorData)
    STATE_DECLARE(InputScanner,	event_received,			NoEventData)

    /* State map to define state object order
         * Each state amp entry defines a state object */
    BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&idle)
		STATE_MAP_ENTRY(&event_received)
    END_STATE_MAP
};

#endif /* INPUTSCANNER_H_ */

#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <string.h>
#include "GarageDoor.h"
#include <string.h>
#include "InputScanner.h"

void* InputScannerThread( void* arg) {
	char char_input;

	InputScanner IS;
	std::cout << "Currently the simulation takes only keyboard input" << std::endl;
	std::cout << "Press 'b' or 'B' to simulate Push_Button Effect" << std::endl;
	std::cout << "Press 'o' or 'O' to simulate Overcurrent Effect" << std::endl;
	std::cout << "Press 'i' or 'I' to simulate Infrared Beam Triggered Effect" << std::endl;


	while (TRUE) {
		std::cout << "Your input: ";
		std::cin >> char_input;

		if (char_input == 'b' || char_input == 'B') {
			GarageDoorData* data = new GarageDoorData();
			data->button_pushed = TRUE;
			IS.Event_Received(data);
		}
		if (char_input == 'o' || char_input == 'O') {
			GarageDoorData* data = new GarageDoorData();
			data->overcurrent = TRUE;
			IS.Event_Received(data);
		}if (char_input == 'i' || char_input == 'I') {
			GarageDoorData* data = new GarageDoorData();
			data->ir_interrupt = TRUE;
			IS.Event_Received(data);
		}

		delay(500);
	}
	return (0);
}


void* GarageDoorThread( void* arg) {
	int rcvid;
	char message[512];
	int chid = (int)arg;
	GarageDoor GD;

	//chid = ChannelCreate (0);

	GD.Init();
	while (TRUE) {
		GarageDoorData* data = new GarageDoorData();
		data->button_pushed = TRUE;
		GD.Operate(data);

		//read
		rcvid = MsgReceive (chid, message, sizeof (message), NULL);
		std::cout << "Receive ID: " << rcvid << std::endl;
		std::cout << "Message: " << message << std::endl;
		//reply
		strcpy(message, "gg");
		MsgReply(rcvid, 0, message, sizeof(message));

		delay(500);
	}
	return (0);
}

void* MockSender( void* arg) {
	int coid;
	int chid = (int)arg;
	const char* smsg = "hmm";
	char rmsg [200];
	coid = ConnectAttach(0,77,chid,0,0);

	if (coid == -1) {
		std::cout << "Connection Failed!" << std::endl;
	}

	if (MsgSend(coid, smsg, strlen(smsg) + 1, rmsg, sizeof (rmsg)) == -1 ){
		std::cout << "Error during MsgSend" << std::endl;
	}

	if (strlen(rmsg) > 0) {
		std::cout << "Process returns " << rmsg << std::endl;
	}
	return (0);
}


int main(int argc, char **argv) {
	int chid = ChannelCreate (0);

	std::cout << "Initializing the Garage Door Simulation" << std::endl;
	/* Prepare thread creation*/
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	/* Thread Ready? */
	pthread_create(NULL, &attr, &GarageDoorThread, (void*)chid);
	pthread_create(NULL, &attr, &MockSender, (void*)chid);
	pthread_create(NULL, &attr, &InputScannerThread, NULL);
	/* Run thread for 10 sec */
	sleep(10);


	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

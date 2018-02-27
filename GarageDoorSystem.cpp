#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <string.h>
#include <process.h>
#include "GarageDoor.h"
#include "InputScanner.h"


void* InputScannerThread( void* arg) {
	char char_input;
	//char* smsg;
	int coid;
	int chid = (int)arg;
	char rmsg [50];
	InputScanner IS;
	coid = ConnectAttach(0,getpid(),chid,0,0);

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
			//strcpy(smsg, "button_pushed");
			MsgSend(coid, (void*)data, sizeof(data), rmsg, sizeof (rmsg));
		}
		if (char_input == 'o' || char_input == 'O') {
			GarageDoorData* data = new GarageDoorData();
			data->overcurrent = TRUE;
			IS.Event_Received(data);
			//strcpy(smsg, "overcurrent");
			MsgSend(coid, (void*)data, sizeof(data), rmsg, sizeof (rmsg));
		}if (char_input == 'i' || char_input == 'I') {
			GarageDoorData* data = new GarageDoorData();
			data->ir_interrupt = TRUE;
			IS.Event_Received(data);
			//strcpy(smsg, "ir_interrupt");
			MsgSend(coid, (void*)data, sizeof(data), rmsg, sizeof (rmsg));
		}


//		if (coid == -1) {
//			std::cout << "Connection Failed!" << std::endl;
//		}
//		if (MsgSend(coid, (void*)data, strlen(smsg) + 1, rmsg, sizeof (rmsg)) == -1 ){
//			std::cout << "Error during MsgSend" << std::endl;
//		}
//		if (strlen(rmsg) > 0) {
//			std::cout << "Process returns " << rmsg << std::endl;
//		}

		delay(100);
	}
	return (0);
}


void* GarageDoorThread( void* arg) {
	int rcvid;
	char message[512];
	GarageDoorData data;
	int chid = (int)arg;
	GarageDoor GD;

	//chid = ChannelCreate (0);

	while (TRUE) {
		GarageDoorData* data = new GarageDoorData();

		//read
		rcvid = MsgReceive (chid, data, sizeof (data), NULL);
		std::cout << "Receive ID: " << rcvid << std::endl;
		//std::cout << "Message: " << message << std::endl;
//		if (message == "button_pushed") {
//			data->button_pushed = TRUE;
//			GD.Operate(data);
//		}
//		if (message == "overcurrent") {
//			data->overcurrent = TRUE;
//			GD.Halt(data);
//		}
//		if (message == "ir_interrupt") {
//			data->ir_interrupt = TRUE;
//			GD.Halt(data);
//		}
		GD.Operate(data);
		//reply
		strcpy(message, "<<Applied>>");
		MsgReply(rcvid, 0, message, sizeof(message));

		delay(100); // MsgReceive waits for this function
	}
	return (0);
}

void* MockSender( void* arg) {
	int coid;
	int chid = (int)arg;
	const char* smsg = "hmm";
	char rmsg [200];
	coid = ConnectAttach(0,getpid(),chid,0,0);
	while(TRUE) {
		if (coid == -1) {
			std::cout << "Connection Failed!" << std::endl;
		}

		if (MsgSend(coid, smsg, strlen(smsg) + 1, rmsg, sizeof (rmsg)) == -1 ){
			std::cout << "Error during MsgSend" << std::endl;
		}

		if (strlen(rmsg) > 0) {
			std::cout << "Process returns " << rmsg << std::endl;
		}
		delay(100);
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
	//pthread_create(NULL, &attr, &MockSender, (void*)chid);
	pthread_create(NULL, &attr, &InputScannerThread, (void*)chid);
	/* Run thread for 600 sec */
	sleep(600);


	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

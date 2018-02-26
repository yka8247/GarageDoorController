#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <string.h>
#include "GarageDoor.h"
//#include "mailbox.h"


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
	/* Run thread for 10 sec */
	sleep(10);


	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

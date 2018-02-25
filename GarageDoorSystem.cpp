#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "GarageDoor.h"


void* GarageDoorThread( void* arg) {
	//TODO: Add GarageDoor functionality here
	GarageDoor GD;
	GD.Init();
	while (TRUE) {
		GarageDoorData* data = new GarageDoorData();
		data->button_pushed = TRUE;
		GD.Operate(data);
	}
	return (0);
}


int main(int argc, char *argv[]) {
	std::cout << "Initializing the Garage Door Simulation" << std::endl;
	/* Prepare thread creation*/
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	/* Thread Ready? */
	pthread_create(NULL, &attr, &GarageDoorThread, NULL);
	/* Run thread for 10 sec */
	sleep(10);


	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

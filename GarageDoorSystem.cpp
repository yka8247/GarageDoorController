#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "GarageDoor.h"

void test() {
	GarageDoor GD;
	GD.Init();
	GarageDoorData* data1 = new GarageDoorData();
	data1->button_pushed = TRUE;
	GD.DoorUp(data1);
	GarageDoorData* data2 = new GarageDoorData();
	data2->button_pushed = TRUE;
	GD.DoorDown(data2);
}

void* GarageDoorThread( void* arg) {
	//TODO: Add GarageDoor functionality here
	test();
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

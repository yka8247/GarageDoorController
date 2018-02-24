#include <cstdlib>
#include <iostream>
#include "GarageDoor.h"

int main(int argc, char *argv[]) {
	std::cout << "Starting the Garage Door Simulation" << std::endl;
	GarageDoor GD;
	GD.Init();
	GarageDoorData* data1 = new GarageDoorData();
	data1->button_pushed = TRUE;
	GD.DoorUp(data1);
	std::cout << "Testing Line: See if this is still alive" << std::endl;
	GarageDoorData* data2 = new GarageDoorData();
	data2->button_pushed = TRUE;
	GD.DoorDown(data2);
	std::cout << "Testing Line: SUCCESSFULLY TESTED" << std::endl;

	return EXIT_SUCCESS;
}

#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <process.h>
#include "GarageDoor.h"
#include "Channels.h"
#include "Hardware.h"


void* InputScannerController(void* args) {
	/* Initialize Message Channels */
	Channels* chids = (Channels*)args;
	int sendCh = chids->gdc_chid;
	int receiveCh = chids->is_chid;
	/* Initialize variables */
	int coid;
	int rcvid;
	char rmsg [100];
	char message[100];
	coid = ConnectAttach(0,getpid(),sendCh,0,0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return (FALSE);
	}

	while (TRUE) {
		/* check if any message arrived */
		rcvid = MsgReceive(receiveCh, message, sizeof(message), NULL);
		if (rcvid) {
			std::cout << "<<Input Scanner>>: " << message[0] << std::endl;
		}
		/* send a response back to the sender */
		std::string response = "InputScanner <<Received>>";
		MsgReply(rcvid, 1, &response, sizeof(response));

		/* forward to controller */
		if(MsgSend(coid, message, strlen(message) + 1, rmsg, sizeof(rmsg)) == -1) {
			std::cout << "Could not send message :[ " << message << std::endl;
		}
		/* Terminate */
		if(message[0] == 't' || message[0] == 'T'){
			std::cout <<"Shut Down::<<InputScanner>>" << std::endl;
			break;
		}
	}
	return (FALSE);
}


void* HardwareController(void* args){
	/* Initialize Message Channels */
	Channels* chids = (Channels*)args;
	int chid = chids->is_chid;
	/* Initialize variables */
	int coid;
	char inp;
	char rmsg[100];
	coid = ConnectAttach(0, getpid(), chid, 0, 0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return 0;
	}
	/* @TODO: Fill up necessary hardware initialization here */

	while(TRUE) {
		/* @TODO: Fill up necessary hardware invocation here */

	}
	return EXIT_SUCCESS;
}



void* KeyboardScannerController(void* args){
	/* Initialize Message Channels */
	Channels* chids = (Channels*)args;
	int chid = chids->is_chid;
	/* Initialize variables */
	int coid;
	char inp;
	char rmsg[100];
	coid = ConnectAttach(0, getpid(), chid, 0, 0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return (FALSE);
	}
	/* Print instruction messages for the keyboard simulation */
	std::cout << "Initializing Keyboard Interrupt Simulation" << std::endl;
	std::cout << "Press 'b' or 'B' to simulate Push_Button Effect" << std::endl;
	std::cout << "Press 'o' or 'O' to simulate OverCurrent Effect" << std::endl;
	std::cout << "Press 'i' or 'I' to simulate Infrared Beam Triggered Effect" << std::endl;
	std::cout << "Press 't' or 'T' to Terminate the Simulation" << std::endl;

	while(TRUE) {
		std::cin >> inp;
		switch(inp) {
		case 'B':
		case 'b': {
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
			break;
		}
		case 'I':
		case 'i': {
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
			break;
		}
		case 'O':
		case 'o': {
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
			break;
		}
		case 'T':
		case 't': {
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
			std::cout << "Shutting down keyboard scanner..." << std::endl;
			return EXIT_SUCCESS;
		}
		default:
			std::cout << "Invalid input" << std::endl;
		}
	}
	return EXIT_SUCCESS;
}


void* GarageDoorController(void* args) {
	/* Initialize Message Channels */
	Channels* chids = (Channels*)args;
	int sendCh = chids->is_chid;
	int receiveCh = chids->gdc_chid;
	/* Initialization */
	GarageDoorData* data;
	GarageDoor GD;
	int coid;
	int rcvid;
	char message[100];
	coid = ConnectAttach(0,getpid(),sendCh,0,0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return (FALSE);
	}

	while (TRUE) {
		// check if any message arrived
		rcvid = MsgReceive (receiveCh, message, sizeof(message), NULL);
		if (rcvid) {
			std::cout << "<<GDC>>: " << message[0] << std::endl;
		}
		// data = KeyboardEventGenerator(message[0]);
		data = HardwareEventGenerator(message[0]);
		if (data->button_pushed) {
			// print for testing purposes
			std::cout << "Button Push Detected" << std::endl;
			GD.Operate(data);
		} else if(data->ir_interrupt) {
			// print for testing purposes
			std::cout << "IR Beam Interruption Detected" << std::endl;
			GD.Halt(data);
		} else if(data->overcurrent) {
			// print for testing purposes
			std::cout << "OverCurrent Detected" << std::endl;
			GD.Halt(data);
		}
		/* send a response back to unlock the mutex */
		std::string response = "<<GDC>> Received";
		MsgReply(rcvid, 1, &response, sizeof(response));
		/* Terminate */
		if(message[0] == 't' || message[0] == 'T'){
			std::cout <<"Shut Down::<<GDC>>" << std::endl;
			break;
		}
	}
	return (FALSE);
}


int main(int argc, char **argv) {
	/* Hardware Initialization */
	uintptr_t ctrlHandle;
	ctrlHandle = mmap_device_io(IO_PORT_SIZE, CTRL_ADDRESS);
	/* Get Full Permission over RTOS */
	if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		std::perror("Failed to get I/O access permission");
		return 1;
	}
	if(ctrlHandle == MAP_DEVICE_FAILED) {
		std::perror("Failed to map control register");
		return 2;
	}
	/* Prepare thread creation*/
	pthread_t keyboardScannerThread;
	pthread_t HardwareScannerThread;
	pthread_t InputScannerThread;
	pthread_t GarageDoorThread;
	/* Prepare channel structure creation */
	int gdc_chid = ChannelCreate(0);
	int is_chid = ChannelCreate(0);
	Channels channels = { is_chid, gdc_chid };
	/* Initialization complete -> Move on to runtime environment */
	std::cout << "Initializing the Garage Door Simulation" << std::endl;
	/* Call Threads */
	pthread_create(&InputScannerThread, NULL, &InputScannerController, (void*)&channels);
	pthread_create(&GarageDoorThread, NULL, &GarageDoorController, (void*)&channels);
	//pthread_create(&keyboardScannerThread, NULL, &KeyboardScannerController, (void*)&channels);
	pthread_create(&HardwareScannerThread, NULL, &HardwareController, (void*)&channels);
	/* Join Threads */
	pthread_join(InputScannerThread, NULL);
	pthread_join(GarageDoorThread, NULL);
	//pthread_join(keyboardScannerThread, NULL);
	pthread_join(HardwareScannerThread, NULL);


	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

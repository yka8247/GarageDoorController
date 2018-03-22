#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
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
	/* Initialize message variables */
	int coid;
	char inp;
	char rmsg[100];
	/* Initialize Message Channels */
	Channels* chids = (Channels*)args;
	int chid = chids->is_chid;
	/* Hardware Initialization */
	uintptr_t ctrlHandle = mmap_device_io(IO_PORT_SIZE, CTRL_ADDRESS);
	uintptr_t paHandle = mmap_device_io(IO_PORT_SIZE, PORT_A_ADDRESS);
	uintptr_t pbHandle = mmap_device_io(IO_PORT_SIZE, PORT_B_ADDRESS);

	if(ctrlHandle == MAP_DEVICE_FAILED) {
		std::perror("Failed to map CTRL register");
		return (0);
	}
	if(paHandle == MAP_DEVICE_FAILED) {
		std::perror("Failed to map PORT_A register");
		return (0);
	}
	if(pbHandle == MAP_DEVICE_FAILED) {
		std::perror("Failed to map PORT_B register");
		return (0);
	}

	// Set control mode << 0001 0000 >> portA = input , portB = output
	out8(ctrlHandle, 0x10);

	/* Attach the message channel */
	coid = ConnectAttach(0, getpid(), chid, 0, 0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return 0;
	}

	//Take FPGA out of reset
	//set A2-12 to "high"


	while(TRUE) {
		/* @TODO: Fill up necessary hardware invocation here */
		// Question 2 : Why doesn't if conditional does not take defined P1~7 from Hardware.h???
		uint8_t inp_sig = in8(paHandle);
		if ( inp_sig & P1 ) {		// got Full_Open Signal
			inp = 'c';
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
		}
		if ( inp_sig & P2 ) {			// got Full_Close Signal
			inp = 'd';
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
		}
		if ( inp_sig & P3 ) {			// got IR_BEAM_BROKEN Signal
			inp = 'i';
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
		}
		if ( inp_sig & P4 ) {			// got OVER_CURRENT Signal
			inp = 'o';
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << inp << std::endl;
			}
		}
		if ( inp_sig & P5 ) {			// got REMOTE_PUSH_BUTTON Signal
			inp = 'b';
			if(MsgSend(coid, &inp, strlen(&inp) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: Remote Push Button" << std::endl;
			}
		}
		delay(500);
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

void GDEvent(GarageDoor GD, GarageDoorData* data, char* message){
	if (data->button_pushed) {
		// print for testing purposes
		std::cout << "Button Push Detected" << std::endl;
		GD.Operate(data);
	} else if(data->full_open_signal) {
		// print for testing purposes
		std::cout << "Full Open Signal Detected" << std::endl;
		GD.Halt(data);
	 }else if(data->full_close_signal) {
		// print for testing purposes
		std::cout << "Full Close Signal Detected" << std::endl;
		GD.Halt(data);
	} else if(data->ir_interrupt) {
		// print for testing purposes
		std::cout << "IR Beam Interruption Detected" << std::endl;
		GD.Halt(data);
	} else if(data->overcurrent) {
		// print for testing purposes
		std::cout << "OverCurrent Detected" << std::endl;
		GD.Halt(data);
	}
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
		data = EventGenerator(message[0]);
		GDEvent(GD, data, message);
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
	/* Get Full Permission over RTOS */
	if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		std::perror("Failed to get I/O access permission");
		return 1;
	}
	/* Prepare thread creation*/
	//pthread_t KeyboardScannerThread;
	pthread_t InputScannerThread;
	pthread_t GarageDoorThread;
	pthread_t HardwareScannerThread;
	/* Prepare channel structure creation */
	int gdc_chid = ChannelCreate(0);
	int is_chid = ChannelCreate(0);
	Channels channels = { is_chid, gdc_chid };
	/* Initialization complete -> Move on to runtime environment */
	std::cout << "Initializing the Garage Door Simulation" << std::endl;
	/* Call Threads */
	pthread_create(&InputScannerThread, NULL, &InputScannerController, (void*)&channels);
	pthread_create(&GarageDoorThread, NULL, &GarageDoorController, (void*)&channels);
	pthread_create(&HardwareScannerThread, NULL, &HardwareController, (void*)&channels);
	//pthread_create(&KeyboardScannerThread, NULL, &KeyboardScannerController, (void*)&channels);
	/* Join Threads */
	pthread_join(InputScannerThread, NULL);
	pthread_join(GarageDoorThread, NULL);
	pthread_join(HardwareScannerThread, NULL);
	//pthread_join(KeyboardScannerThread, NULL);

	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

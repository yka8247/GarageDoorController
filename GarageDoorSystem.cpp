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
#include "ArgObj.h"
#include "Hardware.h"


void* GarageDoorController(void* args) {
	/* Initialize Message Channels */
	ArgObj* arg_obj = (ArgObj*)args;
	int sendCh = arg_obj->is_chid;
	int receiveCh = arg_obj->gdc_chid;
	/* Initialize Hardware Handling port */
	uintptr_t pbHandle = arg_obj->pbHandle;
	/* Initialization */
	GarageDoorData* data;
	GarageDoor GD;
	int coid;
	int rcvid;
	char message[50];
	coid = ConnectAttach(0,getpid(),sendCh,0,0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return (FALSE);
	}

	while (TRUE) {
		// check if any message arrived
		rcvid = MsgReceive (receiveCh, message, sizeof(message), NULL);
//		if (rcvid) {
//			std::cout << "<<GDC>>: " << message[0] << std::endl;
//		}

		/* send a response back to unlock the mutex */
		std::string response = "<<GDC>> Received";
		MsgReply(rcvid, 1, &response, sizeof(response));

		// skip this action if 'x' is received
		if (message[0] != 'x') {
			/* generate data based on the received message */
			data = BuildEvent(message[0], pbHandle);
			/* invoke the state machine with the generated data */
			if (data->button_pushed) {
				// print for testing purposes
				GD.Operate(data);
			} else if(data->full_open_signal) {
				// print for testing purposes
				GD.Complete(data);
			 }else if(data->full_close_signal) {
				// print for testing purposes
				GD.Complete(data);
			} else if(data->ir_interrupt) {
				// print for testing purposes
				GD.Halt(data);
			} else if(data->overcurrent) {
				// print for testing purposes
				GD.Halt(data);
			}
		}
	}
	return (FALSE);
}


void* InputScannerController(void* args) {
	/* Initialize Message Channels */
	ArgObj* arg_obj = (ArgObj*)args;
	int sendCh = arg_obj->gdc_chid;
	int receiveCh = arg_obj->is_chid;
	/* Initialize variables */
	int coid;
	int rcvid;
	char rmsg [50];
	char message[50];
	coid = ConnectAttach(0,getpid(),sendCh,0,0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return (FALSE);
	}

	while (TRUE) {
		/* check if any message arrived */
		rcvid = MsgReceive(receiveCh, message, sizeof(message), NULL);
//		if (rcvid) {
//			std::cout << "<<Input Scanner>>: " << message[0] << std::endl;
//		}

		/* send a response back to the sender */
		std::string response = "InputScanner <<Received>>";
		MsgReply(rcvid, 1, &response, sizeof(response));

		/* forward the message to controller */
		if(MsgSend(coid, message, strlen(message) + 1, rmsg, sizeof(rmsg)) == -1) {
			std::cout << "Could not send message :[ " << message << std::endl;
		}
	}
	return (FALSE);
}


void* HardwareController(void* args){
	/* Take argument from thread */
	ArgObj* arg_obj = (ArgObj*)args;
	/* declare temporary signal variable */
	static uint8_t prev_sig = 0;
	/* Initialize message variables */
	int coid;
	char in_char;
	char rmsg[50];
	int chid = arg_obj->is_chid;
	/* Hardware Initialization */
	uintptr_t ctrlHandle = arg_obj->ctrlHandle;
	uintptr_t paHandle = arg_obj->paHandle;
	uintptr_t pbHandle = arg_obj->pbHandle;

	/* Attach the message channel */
	coid = ConnectAttach(0, getpid(), chid, 0, 0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return 0;
	}

	// Set control mode << 0001 0000 >> portA = input , portB = output
	out8(ctrlHandle, P5);

	// Set Port B pin 8 to high to handle active low reset signal
	out8(pbHandle, P8);

	while(TRUE) {
		/* gather signal and check if it is same with previous signal */
		uint8_t inp_sig = in8(paHandle);

		/* process signal reading if previous singal is different then current signal */
		if (prev_sig != inp_sig){
			/* Send detected signal to the message queue  */
			if ( (inp_sig & P5) && ((prev_sig & P5) == FALSE) ) {				// REMOTE_PUSH_BUTTON Signal detected
				in_char = 'p';
			}
			else if ( (inp_sig & P4)  && ((prev_sig & P4) == FALSE) ) {			// OVER_CURRENT Signal detected
				in_char = 'v';
			}
			else if ( (inp_sig & P3) && ((prev_sig & P3) == FALSE) ) {			// IR_BEAM_BROKEN Signal detected
				in_char = 'i';
			}
			else if ( (inp_sig & P2) && ((prev_sig & P2) == FALSE) ) {			// Full_Close Signal detected
				in_char = 'c';
			}
			else if ( (inp_sig & P1) && ((prev_sig & P1) == FALSE) ) {			// Full_Open Signal detected
				in_char = 'o';
			} else {
				// otherwise, send a error message
				in_char = 'x';
			}
			if (MsgSend(coid, &in_char, strlen(&in_char) + 1, rmsg, sizeof(rmsg)) == -1) {
				std::cout << "Failed to send a message :: " << in_char << std::endl;
			}
		}
		/* set prev_sig to prevent redundant process */
		prev_sig = inp_sig;
		usleep(500);
	}
	return EXIT_SUCCESS;
}


int main(int argc, char **argv) {
	/* Get Full Permission over RTOS */
	if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		std::perror("Failed to get I/O access permission");
		return 1;
	}
	/* Map Hardware ports */
	uintptr_t ctrlHandle = mmap_device_io(IO_PORT_SIZE, CTRL_ADDRESS);
	uintptr_t paHandle = mmap_device_io(IO_PORT_SIZE, PORT_A_ADDRESS);
	uintptr_t pbHandle = mmap_device_io(IO_PORT_SIZE, PORT_B_ADDRESS);
	/* check for successful hardware initialization */
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
	/* Prepare thread creation*/
	pthread_t InputScannerThread;
	pthread_t GarageDoorThread;
	pthread_t HardwareScannerThread;
	/* Prepare channel structure creation */
	int gdc_chid = ChannelCreate(0);
	int is_chid = ChannelCreate(0);
	ArgObj arg_obj = { is_chid, gdc_chid, ctrlHandle, paHandle, pbHandle };
	/* Initialization complete -> Move on to runtime environment */
	std::cout << "Initializing the Garage Door Simulation" << std::endl;
	/* Call Threads */
	pthread_create(&InputScannerThread, NULL, &InputScannerController, (void*)&arg_obj);
	pthread_create(&GarageDoorThread, NULL, &GarageDoorController, (void*)&arg_obj);
	pthread_create(&HardwareScannerThread, NULL, &HardwareController, (void*)&arg_obj);

	while(1){usleep(1);};
	/* Join Threads */
	pthread_join(InputScannerThread, NULL);
	pthread_join(GarageDoorThread, NULL);
	pthread_join(HardwareScannerThread, NULL);

	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <string.h>
#include <process.h>
#include "GarageDoor.h"
#include "InputScanner.h"
#include "Channels.h"


void* InputScannerController( void* args) {
	/* Initialize Message Channels */
	Channels* chids = (Channels*)args;
	int sendCh = chids->gdc_chid;
	int receiveCh = chids->is_chid;
	/* Initialize variables */
	InputScanner IS;
	int coid;
	int rcvid;
	char rmsg [100];
	char message[100];
	coid = ConnectAttach(0,getpid(),sendCh,0,0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return 0;
	}

	while (TRUE) {
		/* check if any message arrived */
		rcvid = MsgReceive(receiveCh, message, sizeof(message), NULL);
		if (rcvid) {
			std::cout << "Arrived message <<Input Scanner>>: " << message << std::endl;
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
//		if (char_input == 'b' || char_input == 'B') {
//			GarageDoorData* data = new GarageDoorData();
//			data->button_pushed = TRUE;
//			IS.Event_Received(data);
//			//strcpy(smsg, "button_pushed");
//			MsgSend(coid, (void*)data, sizeof(data), rmsg, sizeof (rmsg));
//		}
	}
	return (0);
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
		return 0;
	}
	/* Print instruction messages for the keyboard simulation */
	std::cout << "Initializing Keyboard Interrupt Simulation" << std::endl;
	std::cout << "Press 'b' or 'B' to simulate Push_Button Effect" << std::endl;
	std::cout << "Press 'o' or 'O' to simulate OverCurrent Effect" << std::endl;
	std::cout << "Press 'i' or 'I' to simulate Infrared Beam Triggered Effect" << std::endl;
	std::cout << "Press 't' or 'T' to Terminate the Simulation" << std::endl;

	while(1) {
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


void* GarageDoorController( void* args) {
	/* Initialize Message Channels */
	Channels* chids = (Channels*)args;
	int sendCh = chids->is_chid;
	int receiveCh = chids->gdc_chid;
	/* Initialization */
	//GarageDoorData* data;
	//GarageDoor GD;
	int coid;
	int rcvid;
	char message[100];
	coid = ConnectAttach(0,getpid(),sendCh,0,0);
	if (coid == -1) {
		std::cout << "Connection Failed" << std::endl;
		return 0;
	}

	while (TRUE) {
		// check if any message arrived
		rcvid = MsgReceive (receiveCh, message, sizeof(message), NULL);
		if (rcvid) {
			std::cout << "Arrived message <<GDC>>: " << message << std::endl;
		}
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
//		GD.Operate(data);
		/* send a response back to the sender */
		std::string response = "<<GDC>> Received";
		MsgReply(rcvid, 1, &response, sizeof(response));
		/* Terminate */
		if(message[0] == 't' || message[0] == 'T'){
			std::cout <<"Shut Down::<<GDC>>" << std::endl;
			break;
		}
	}
	return (0);
}

//void* MockSender( void* arg) {
//	int coid;
//	int chid = (int)arg;
//	const char* smsg = "hmm";
//	char rmsg [200];
//	coid = ConnectAttach(0,getpid(),chid,0,0);
//	while(TRUE) {
//		if (coid == -1) {
//			std::cout << "Connection Failed!" << std::endl;
//		}
//
//		if (MsgSend(coid, smsg, strlen(smsg) + 1, rmsg, sizeof (rmsg)) == -1 ){
//			std::cout << "Error during MsgSend" << std::endl;
//		}
//
//		if (strlen(rmsg) > 0) {
//			std::cout << "Process returns " << rmsg << std::endl;
//		}
//		delay(100);
//	}
//	return (0);
//}


int main(int argc, char **argv) {
	/* Get Full Permission over RTOS */
	if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		std::perror("Failed to get I/O access permission");
		return 1;
	}
	/* Prepare thread creation*/
	pthread_t keyboardScannerThread;
	pthread_t inputScannerThread;
	pthread_t GarageDoorThread;
	/* Prepare channel structure creation */
	int gdc_chid = ChannelCreate(0);
	int is_chid = ChannelCreate(0);
	Channels channels = { is_chid, gdc_chid };
	/* Initialization complete -> Move on to runtime environment */
	std::cout << "Initializing the Garage Door Simulation" << std::endl;
	/* Call Threads */
	pthread_create(&inputScannerThread, NULL, &InputScannerController, (void*)&channels);
	pthread_create(&GarageDoorThread, NULL, &GarageDoorController, (void*)&channels);
	pthread_create(&keyboardScannerThread, NULL, &KeyboardScannerController, (void*)&channels);
	/* Join Threads */
	pthread_join(inputScannerThread, NULL);
	pthread_join(GarageDoorThread, NULL);
	pthread_join(keyboardScannerThread, NULL);

	std::cout << "Terminating the Garage Door Simulation" << std::endl;
	return EXIT_SUCCESS;
}

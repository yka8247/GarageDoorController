/*
 * Hardware.h
 *
 *  Created on: Mar 14, 2018
 *      Author: yxa8247
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#define IO_PORT_SIZE 	1
#define CTRL_ADDRESS 	0x28B	// Base + 11
#define PORT_A_ADDRESS 	0x288	// Base + 8
#define PORT_B_ADDRESS 	0x289	// Base + 9

/* Input Masks */
#define P1				0x01	// Pin #0
#define P2				0x02	// Pin #1
#define P3				0x04	// Pin #2
#define P4				0x08	// Pin #3
#define P5				0x10	// Pin #4
#define P6				0x20	// Pin #5
#define P7				0x40	// Pin #6
#define P8				0x80	// Pin #7

#endif /* HARDWARE_H_ */

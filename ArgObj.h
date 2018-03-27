/*
 * Channels.h
 *
 *  Created on: Mar 13, 2018
 *      Author: Yongki An
 */

#ifndef CHANNELS_H_
#define CHANNELS_H_
#include "GarageDoor.h"

struct ArgObj {
	int is_chid;
	int gdc_chid;
	uintptr_t ctrlHandle;
	uintptr_t paHandle;
	uintptr_t pbHandle;
};

#endif /* CHANNELS_H_ */

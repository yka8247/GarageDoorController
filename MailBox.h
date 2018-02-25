/*
 * MailBox.h
 *
 *  Created on: Feb 24, 2018
 *      Author: yxa8247
 */

#ifndef MAILBOX_H_
#define MAILBOX_H_
#include <queue.h>
#include <pthread.h>
#include "StateMachine.h"

class MailBox {
private:
	pthread_mutex_t enq_mutex;
	pthread_mutex_t deq_mutex;
	pthread_cond_t cond;
public:
	void enqueue(const EventData& data);
	void dequeue();
	BOOL empty();
	EventData& front();

};

#endif /* MAILBOX_H_ */

//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		kernelse.h
//	Contains:	KernelSem class interface
//

#ifndef _KERNELSE_H_
#define _KERNELSE_H_

#include "pcblist.h"
#include "pcbtlist.h"
#include "pcb.h"

typedef unsigned int Time;
class PCBList;

class KernelSem {

public:

	KernelSem(int init = 1);
	virtual ~KernelSem();

	virtual int wait(Time maxTimeToWait);
	virtual void signal();

	void semTick();

	int val() const;  						// Returns the current value of the semaphore

private:

	int value;

	PCBList* unlimitedWaitList;
	PCBTimedList* timeLimitedList;

};

#endif /* _KERNELSE_H_ */

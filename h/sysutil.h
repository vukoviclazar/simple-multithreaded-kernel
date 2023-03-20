//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		sysutil.h
//	Contains: 	SysUtil class interface (a container class containing variables and functions used for implementing the 'kernel')
//

#ifndef _SYSUTIL_H_
#define _SYSUTIL_H_

#include <dos.h>
#include "thread.h"
#include "pcblist.h"
#include "semlist.h"
#include "ivtentry.h"

class PCB;
class PCBList;
class SemList;

// macro for disabling interrupts, only to be used when soft_lock cannot be used
// push PSW onto the stack
// set PSW I bit to 0
// MAKE SURE THAT THE PSW IS REMOVED FROM THE STACK
#define hard_lock \
	asm { \
	pushf; \
	cli; \
	}

// restore PSW from the stack,
// MAKE SURE THAT THE PSW IS ACTUALLY ON TOP OF THE STACK
#define hard_unlock \
	asm popf;

// macro for disabling context switch without disabling interrupts
// disable interrupts and increment the globalLock flag
// enable interrupts again, the hard_lock is done in case an interrupt
// happens while assigning value to the variable globalLock
#define soft_lock \
	hard_lock \
	SysUtil::globalLock++; \
	hard_unlock \

// disable interrupts
// decrement the globalLock flag, context will now be allowed to change
// if this was the last nested unlock and the globalLock value is 0
// enable interrupts again, the hard_lock is done in case an interrupt
// happens while assigning value to the variable globalLock
// if there was an interrupt during the critical section
// then change the context
#define soft_unlock \
		hard_lock \
		if (--SysUtil::globalLock == 0) { \
			if (SysUtil::requestWhileLocked == REQUESTED) {\
				SysUtil::requestWhileLocked = NOT_REQUESTED;\
				PCB::dispatch();\
			} \
		} \
		hard_unlock; \


typedef void interrupt (*pInterrupt) (...);								// declaration needed for using setvect and getvect functions from dos.h

typedef unsigned lock;													// type for globalLock

enum ContextSwitchRequest { NOT_REQUESTED, REQUESTED };


class SysUtil {															// class containing only static functions and variables that are generally useful

public:

	static void initializeIVT();										// used for modifying the IVT
	static void restoreIVT();											// used for restoring the old IVT timer entry
	static void interrupt newRoutine(...);								// new interrupt routine, used for context switch

	static void softLock();												// wrapper functions for soft_lock/soft_unlock macros
	static void softUnlock();

	static void putRunning();											// wrappers for Scheduler::put and Scheduler::get, considering the status of the running thread
	static PCB* getRunning();

	static void idleBody();												// body of the idle thread
	static void returnToMain();

	static volatile lock globalLock;									// global lock for disabling context switch, when equal to 0 it is unlocked,
																		// every lock increments it so locks can be nested, lock and unlock should
																		// always be paired, beware if trying to switch context from a nested lock

	static volatile ContextSwitchRequest dispatchRequest;				// global flag for communicating that a context switch is explicitly requested
	static volatile ContextSwitchRequest requestWhileLocked;			// global flag set when a context switch is requested while globalLock is locked

	static volatile Time timeLeftCounter;								// variable that keeps count of how many time increments of processor time
																		// the current thread has left

	static pInterrupt oldRoutine;										// pointer to the old interrupt routine used for redirecting IVT entries

	volatile static PCBList* allThreads;								// list of all created threads
	volatile static PCBList* activeThreads;								// list of threads have status flags set to READY or BLOCKED

	volatile static SemList* allSemaphores;								// list of all semaphores in the program, used for time-restriced wait

	static PCB* mainThread;												// main thread context
	static PCB* idleThread;												// thread which only runs if there are no other READY threads, when there
																		// are no active threads left, idleThread returns context to mainThread

private:

	SysUtil() {};

};

extern volatile unsigned tempSp, tempSs, tempBp;						// helper variables for saving current and loading new context

void tick();															// function called on every timer interrupt

#endif /* _SYSUTIL_H_ */

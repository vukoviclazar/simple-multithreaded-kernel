//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		sysutil.cpp
//	Contains: 	SysUtil class implementation (a container class containing variables and functions used for implementing the 'kernel')
//

#include "sysutil.h"
#include "pcb.h"
#include "schedule.h"
#include <iostream.h>

volatile lock SysUtil::globalLock = 0;														// starting unlocked

volatile ContextSwitchRequest SysUtil::dispatchRequest = NOT_REQUESTED;
volatile ContextSwitchRequest SysUtil::requestWhileLocked = NOT_REQUESTED;

volatile Time SysUtil::timeLeftCounter = 0;

pInterrupt SysUtil::oldRoutine = 0;															// initialized in function initializeIVT

SemList sems;																				// static initialization in case there are static objects of classes
PCBList all, active;																		// Thread or Semaphore
PCB mainpcb, idle(defaultStackSize, defaultTimeSlice);										// it would be better if these were volatile

volatile PCBList* SysUtil::allThreads = &all;
volatile PCBList* SysUtil::activeThreads = &active;

volatile SemList* SysUtil::allSemaphores = &sems;

PCB* SysUtil::mainThread = &mainpcb;
PCB* SysUtil::idleThread = &idle;															// stack size and time slice are not important

volatile PCB* PCB::running = &mainpcb;														// in the beginning main is the running thread

extern volatile unsigned tempSp = 0;
extern volatile unsigned tempSs = 0;
extern volatile unsigned tempBp = 0;


void SysUtil::softLock() {
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif
}

void SysUtil::softUnlock() {
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

void SysUtil::putRunning() {																// if the thread can continue it is put into the scheduler
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif
	if (PCB::running->status == READY) Scheduler::put((PCB*)PCB::running);					// lock is not necessary because this function is called only from the interrupt
																							// routine, but it is there for clarity
#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif
}
PCB* SysUtil::getRunning() {
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif
																							// if there is a user thread in the scheduler it is
	PCB* temp = Scheduler::get();															// chosen to run, if there are no user threads ready,
	if (temp == 0)																			// idleThread should run until they are
		temp = (SysUtil::idleThread);														// lock is not needed, see putRunning
#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif
	return temp;

}

void SysUtil::idleBody() {
	while (!(((PCBList*)SysUtil::activeThreads)->isEmpty())) {								// loop and do nothing until there are no active threads left
//		SysUtil::softLock();
//		cout << "Idle thread " << endl;
//		SysUtil::softUnlock();
//		for (int j = 0; j < 10000; j++)
//			for( int k = 0; k < 30000; k++);
	}
	SysUtil::returnToMain();																// when no active threads are left return to main
}

void SysUtil::returnToMain() {
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif
//	cout << "Return to main function " << endl;
//	asm cli;

	Scheduler::put(SysUtil::mainThread);													// mainThread will be the only one in the scheduler
	PCB::dispatch();
#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif
}

void SysUtil::initializeIVT() {

#ifndef BCC_BLOCK_IGNORE
	hard_lock

	oldRoutine = getvect(0x08);
	setvect(0x08, SysUtil::newRoutine);
	setvect(0x60, SysUtil::oldRoutine);

	hard_unlock
#endif
}

void SysUtil::restoreIVT() {

#ifndef BCC_BLOCK_IGNORE
	hard_lock

//	SysUtil::oldRoutine = getvect(0x60);
	setvect(0x08, SysUtil::oldRoutine);

	hard_unlock
#endif
}

void interrupt SysUtil::newRoutine(...) {

	if ( SysUtil::dispatchRequest == NOT_REQUESTED) {					// in case the routine is triggered by the hardware timer

		if (PCB::running->timeSlice != 0) {
			SysUtil::timeLeftCounter--;									// decrement the global counter only if the running thread has
		}																// a limited running time

		((SemList*)SysUtil::allSemaphores)->tickAll();					// update time-limited waiting lists for all semaphores

#ifndef BCC_BLOCK_IGNORE
		asm int 0x60;													// call old interrupt routine
#endif
		tick();
	}

	if ( (SysUtil::dispatchRequest == REQUESTED) ||
			( (PCB::running->timeSlice != 0) && (SysUtil::timeLeftCounter == 0) ) )
		{															// in case the running thread ran out of time (and is time-restricted)
																	// or a context switch is required for some other reason (dispatch is called)
		if (SysUtil::globalLock == 0) {								// if context is not locked switch context

			dispatchRequest = NOT_REQUESTED;						// in this block context switch happens, so this flag should be reset

#ifndef BCC_BLOCK_IGNORE											// use helper variables to transfer values from important registers
			asm {
				mov tempSp, sp
				mov tempSs, ss
				mov tempBp, bp
			}
#endif

			PCB::running->sp = tempSp;								// save the context of the running thread
			PCB::running->ss = tempSs;
			PCB::running->bp = tempBp;

//			cout << "Interrupt routine, currently running is thread " << PCB::getRunningId() << endl;
//			asm cli;

			SysUtil::putRunning();
			PCB::running = SysUtil::getRunning();


//			cout << "Context switch, new running is thread " <<  PCB::getRunningId() << endl;
//			asm cli;

			tempSp = PCB::running->sp;								// load the context of the new running thread
			tempSs = PCB::running->ss;
			tempBp = PCB::running->bp;

			SysUtil::timeLeftCounter = PCB::running->timeSlice;		// start timer countdown

#ifndef BCC_BLOCK_IGNORE											// use helper variables to transfer values to important registers
			asm {
				mov sp, tempSp
				mov ss, tempSs
				mov bp, tempBp
			}
#endif
		}
		else {

			dispatchRequest = NOT_REQUESTED;						// context switch did not happen, but the flag should be reset so the timer continues
																	// ticking, when the lock is unlocked requestWhileLocked flag will be enough

			SysUtil::requestWhileLocked = REQUESTED;				// flag that a context switch is required once the globalLock is unlocked
		}
	}
}



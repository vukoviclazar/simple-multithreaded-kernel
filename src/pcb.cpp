//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		pcb.cpp
//	Contains: 	PCB class implementation
//

#include "pcb.h"
#include "sysutil.h"
#include "schedule.h"
#include <iostream.h>
#include <STDLIB.H>

ID PCB::idCount = 0;

PCB::PCB(StackSize stackSize, Time timeSlice, Thread* myThread) {
#ifndef BCC_BLOCK_IGNORE																						// lock context because of operator new and accessing shared data
	soft_lock
#endif

//	cout << stackSize << endl;

	this->stackSize = (stackSize > maxStackSize) ? maxStackSize/sizeof(unsigned) : stackSize/sizeof(unsigned) ;	// since stackSize is in bytes and
																												// on the stack are unsigned values
//	cout << this->stackSize << endl;

	this->timeSlice = timeSlice;
	this->myThread = myThread;
	this->stack = 0;																							// stack is created in the start method
	this->kernelSemRetVal = 0;																					// default value

	this->myBlockedList = new PCBList();
	this->myChildrenList = new PCBList();

	this->myID = idCount++;																						// use current idCount and update it

	this->status = CREATED;
	((PCBList*)SysUtil::allThreads)->add(this);																	// add thread to the global list

#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif

}

PCB::PCB(StackSize stackSize, Time timeSlice) {
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif

	this->stackSize = (stackSize > maxStackSize) ? maxStackSize/sizeof(unsigned) : stackSize/sizeof(unsigned) ;
	this->timeSlice = timeSlice;
	this->myThread = 0;
	this->myID = idCount++;
	this->stack = 0;
	this->kernelSemRetVal = 0;
	this->myBlockedList = 0;
	this->myChildrenList = 0;

	initializeStack(SysUtil::idleBody);							// when the idleThread acquires the CPU it will start the idleBody function

	this->status = DONE;										// this is irrelevant

#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif

}
PCB::PCB() {
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif

	this->stackSize = 0;
	this->stack = 0;
	this->timeSlice = 0;
	this->myThread = 0;
	this->myBlockedList = 0;
	this->myChildrenList = 0;
	this->myID = idCount++;
	this->status = DONE;
	this->kernelSemRetVal = 0;									// all of this is irrelevant, mainThread only needs values of the registers

#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

PCB::~PCB() {
#ifndef BCC_BLOCK_IGNORE										// lock context because of operator delete
	soft_lock
#endif

	((PCBList*)SysUtil::allThreads)->removeByID(this->myID);	// remove the thread from the global list
	delete myBlockedList;
	delete myChildrenList;
	delete [] stack;											// delete everything dynamically allocated

#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

void PCB::start() {												// disable interrupts because of accessing shared list activeThreads and scheduler method
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif

	this->initializeStack(PCB::wrapper);						// create a stack, when the thread acquires the CPU it will start the wrapper function

	this->status = READY;
	((PCBList*)SysUtil::activeThreads)->add(this);				// thread is now ready to be run, add it to the list of active threads and to the scheduler
	Scheduler::put(this);

#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif

}

void PCB::waitToComplete() {
																				// disabling interrupts because the globalLock is changing
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif

	if (this->status == BLOCKED || this->status == READY) {						// if the thread is started but not yet finished,
																				// suspend the calling thread
																				// (here it could be checked if the thread is trying to block itself adn some other edge cases)

//		cout << "Thread " << getRunningId() << " is blocked! Waiting for thread " << myID << endl;
//		asm cli;

		PCB::running->status = BLOCKED;											// suspending
		this->myBlockedList->add((PCB*)PCB::running);

		lock temp = SysUtil::globalLock;										// transfer the value of the globalLock for later
		SysUtil::globalLock = 0;												// reseting the globalLock so context switch can occur

		PCB::dispatch();														// switch context

		SysUtil::globalLock = temp;												// returning the globalLock to its original value

	}																			// if the thread is just CREATED or DONE there is nothing to wait for

#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif

}

ID PCB::getId() {
	return myID;
}

ID PCB::getRunningId() {
	return ((PCB*)running)->myID;
}

Thread * PCB::getThreadById(ID id) {
#ifndef BCC_BLOCK_IGNORE														// lock context because off accessing allThreads
	soft_lock
#endif

	PCB* temp = ((PCBList*)SysUtil::allThreads)->getByID(id);
	if (temp != 0) {
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
		return temp->myThread;
	}
	else {
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
	return 0;
	}
}

void PCB::wrapper() {

	PCB::running->myThread->run();												// call the body of the thread

#ifndef BCC_BLOCK_IGNORE														// disable interrupts until it is safe to switch context
	hard_lock
#endif

	PCB* temp;

//	cout << "Thread " << getRunningId() << " is finished! " << endl;
//	asm cli;


	while (!(((PCB*)PCB::running)->myBlockedList->isEmpty())) {					// unblock all threads waiting for the current thread
		temp = ((PCB*)PCB::running)->myBlockedList->remove();
		temp->status = READY;
		Scheduler::put(temp);

//		cout << " Unblocking thread " << temp->myID << endl;
//		asm cli;

	}

	running->status = DONE;														// this thread is DONE now, remove it from the active thread list
	((PCBList*)SysUtil::activeThreads)->removeByID(getRunningId());

	PCB::dispatch();															// thread is done, switch context

#ifndef BCC_BLOCK_IGNORE														// will never reach this part but it does not matter
	hard_unlock
#endif

}

void PCB::initializeStack(void (*f)()) {

#ifndef BCC_BLOCK_IGNORE														// lock context because of operator new
	soft_lock
#endif

	this->stack = new unsigned[this->stackSize];								// allocate memory for the stack
//	if(!this->stack) abort();													// there should have been memory allocation checks after every new operator

#ifndef BCC_BLOCK_IGNORE

	stack[stackSize - 1] = 0x200;												// PSW register, I bit is set
	stack[stackSize - 2] = FP_SEG(f);											// segment part of the function that is to be executed (code segment)
	stack[stackSize - 3] = FP_OFF(f);											// offset part of the function that is to be executed (instruction pointer)

	stack[stackSize - 12] = 0;													// base pointer value that will be popped from the
																				// stack when returning from the interrupt routine
	ss = FP_SEG(stack + stackSize - 12);
	sp = FP_OFF(stack + stackSize - 12);										// the value is 12 because of the registers saved when
	bp = FP_OFF(stack + stackSize - 12);										// entering the interrupt routine

#endif

#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}


void PCB::dispatch() {
#ifndef BCC_BLOCK_IGNORE
	hard_lock																	// disable interrupts
#endif
	SysUtil::dispatchRequest = REQUESTED;
#ifndef BCC_BLOCK_IGNORE
	SysUtil::newRoutine();														// call modified timer interrupt routine
	hard_unlock																	// return PSW I bit to previous value
#endif
}

//========================//
// added for fork support
//========================//

void PCB::exit() {
#ifndef BCC_BLOCK_IGNORE														// disable interrupts until it is safe to switch context
	hard_lock
#endif

	PCB* temp;																	// this part of code is identical to the wrapper function after return from the run method

//	cout << "Thread " << getRunningId() << " is finished! " << endl;
//	asm cli;


	while (!(((PCB*)PCB::running)->myBlockedList->isEmpty())) {					// unblock all threads waiting for the current thread
		temp = ((PCB*)PCB::running)->myBlockedList->remove();
		temp->status = READY;
		Scheduler::put(temp);

//		cout << " Unblocking thread " << temp->myID << endl;
//		asm cli;

	}

	running->status = DONE;														// this thread is DONE now, remove it from the active thread list
	((PCBList*)SysUtil::activeThreads)->removeByID(getRunningId());

	PCB::dispatch();															// thread is done, switch context

#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif
}

void PCB::waitForForkChildren() {												// block the running thread until all of its children are finished

	PCB* temp;

	while(!(((PCB*)PCB::running)->myChildrenList->isEmpty())) {					// iterate through the whole list, and wait for each child thread to be done
		temp = ((PCB*)PCB::running)->myChildrenList->remove();
		temp->waitToComplete();
	}

}

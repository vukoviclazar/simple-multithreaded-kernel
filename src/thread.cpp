//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		thread.cpp
//	Contains: 	Thread class implementation
//

#include "thread.h"
#include "pcb.h"
#include <IOSTREAM.H>

																// all methods are defined as wrappers for respective methods of the PCB class
																// fork and related methods are defined in the fork.cpp file

Thread::Thread(StackSize stackSize, Time timeSlice) {
#ifndef BCC_BLOCK_IGNORE										// context lock because of new operator
	soft_lock
#endif
	myPCB = new PCB(stackSize, timeSlice, this);
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

Thread::~Thread() {
#ifndef BCC_BLOCK_IGNORE										// context lock because of delete operator
	soft_lock
#endif
	waitToComplete();
	delete myPCB;
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

void Thread::start() {
#ifndef BCC_BLOCK_IGNORE										// lock just in case...
	soft_lock
#endif
	myPCB->start();
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

void Thread::waitToComplete() {
#ifndef BCC_BLOCK_IGNORE										// lock just in case...
	soft_lock
#endif
	myPCB->waitToComplete();
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

ID Thread::getId() {
	return myPCB->getId();
}

 ID Thread::getRunningId() {
	return PCB::getRunningId();
}

 Thread* Thread::getThreadById(ID id) {
	 return PCB::getThreadById(id);
 }

 void dispatch() {
	PCB::dispatch();
 }

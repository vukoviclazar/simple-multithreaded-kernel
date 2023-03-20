//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		pcb.h
//	Contains: 	PCB class interface
//

#ifndef _PCB_H_
#define _PCB_H_

#include "thread.h"
#include "pcblist.h"

const StackSize maxStackSize = 0x10000;				// upper limit of memory allocated for the stack in BYTES, 64KB

enum ThreadState { CREATED, READY, BLOCKED, DONE };	// possible states of a thread, while not yet started its state is CREATED, while its
													// run method is in progress it is either READY or BLOCKED, and when run
													// terminates the thread is DONE

class PCBList;										// just a declaration, since the IDE would not recognize PCBList as a type

class PCB {

public:

	PCB( StackSize, Time, Thread* );				// constructor used when creating instances of Thread class
	PCB( StackSize, Time );							// constructor used only for SysUtil::idleThread
	PCB();											// constructor used only for SysUtil::mainThread
	~PCB();

	void start();
	void waitToComplete();

	ID getId();
	static ID getRunningId();
	static Thread * getThreadById(ID id);

	static void dispatch();							// function used for switching processor context in all cases except
													// when a thread runs out of time

	static volatile PCB* running;					// thread that is currently running on the processor


	volatile ThreadState status;					// current status of the thread
	volatile unsigned sp, ss, bp;					// registers saved for thread context
	Time timeSlice;									// value indicating how much processor time the thread will recieve (at best)
	StackSize stackSize;							// size of memory allocated for the stack (in sizeof(unsigned))


	int kernelSemRetVal;							// return value of the method wait in class KernelSem

//=====================================//
// added for fork support
//=====================================//


	friend ID Thread::fork();
	friend void interrupt splitStack(...);			// these functions need access to some of the private members of class PCB so they are friends
	friend Thread* cloneWrapper();					// they are static so it is safe

	static void exit();
	static void waitForForkChildren();

	PCBList* myChildrenList;						// list of child threads

//=====================================//
// added for fork support
//=====================================//


protected:

private:

	static void wrapper();							// function used for calling the run method of the thread

	void initializeStack(void (*)());				// function used for allocating and initializing the thread stack

	unsigned* stack;								// pointer to memory allocated for the stack
	ID myID;										// unique identifier of the thread
	Thread* myThread;								// pointer to the instance of Thread class associated with this PCB

	PCBList* myBlockedList;							// list of threads blocked waiting for this thread to finish

	static ID idCount;								// variable used for initializing the ID field

};


#endif /* _PCB_H_ */

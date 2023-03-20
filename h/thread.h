//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		thread.h
//	Contains: 	Thread class interface
//

#ifndef _THREAD_H_
#define _THREAD_H_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

typedef unsigned int Time; 				// time, x 55ms
const Time defaultTimeSlice = 2; 		// default = 2*55ms

typedef int ID;

class PCB;	// Kernel's implementation of a user's thread

class Thread {

public:

	void start();
	void waitToComplete();

	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread * getThreadById(ID id);

//=====================================================================//
// extension of the class Thread for the second part of the assignment
//=====================================================================//

public:

	static ID fork();
	static void exit();
	static void waitForForkChildren();

	virtual Thread* clone() const;

//=====================================================================//
// end of extension
//=====================================================================//

protected:

	friend class PCB;
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice =	defaultTimeSlice);
	virtual void run() {}

private:

	PCB* myPCB;

};

void dispatch();

#endif /* _THREAD_H_ */

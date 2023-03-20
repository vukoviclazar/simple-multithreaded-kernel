//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		fork.cpp
//	Contains: 	extensions of Thread class used to enable fork functionality
//

#include "thread.h"
#include "pcb.h"
#include <dos.h>
#include <IOSTREAM.H>
#include <STRING.H>

struct ThreadNode {
	Thread* data;
	ThreadNode* next;
	ThreadNode(Thread* data, ThreadNode* next = 0): data(data), next(next) {}
};

class ThreadList {																	// this class is used only to record Thread objects allocated with new in the method clone
																					// in order to be able to delete them in the end
private:
	ThreadNode* head;
public:
	ThreadList(): head(0) {}
	~ThreadList();
	void add(Thread* newThread);
};

ThreadList::~ThreadList(){															// delete every ThreadNode after deleting the actual Thread it is associated with
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif
		ThreadNode* current = head;
		ThreadNode* old = 0;
		while (current != 0) {
			old = current;
			current= current->next;

//			cout << endl << "deleted" << endl;
//			asm cli;

			delete old->data;
			delete old;
		}
		head = 0;
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

void ThreadList::add(Thread* newThread){											// the Thread is added at the start for simplicity
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif
		head = new ThreadNode(newThread, head);

//		cout << endl << "added" << endl;
//		asm cli;

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

ThreadList toBeDeleted;																// list of Threads that should be deleted once the program terminates

unsigned helpSp, helpSs, topOffset, srcBottomOffset, dstBottomOffset, curBp;
PCB* src, *dst;

void interrupt splitStack(...) {										// copy the stack, update all bp values on the stack,
																		// (update return value on the dst stack),
																		// set sp, bp and ss fields of dst PCB, set its status and put it in the scheduler

	dst->stack = new unsigned[dst->stackSize];							// allocate a stack for the child thread

	memcpy(dst->stack, src->stack, dst->stackSize*sizeof(unsigned));	// could be optimized, not all stackSize*2 bytes need to be copied

#ifndef BCC_BLOCK_IGNORE
	asm {
		mov helpSp, sp
		mov helpSs, ss
	}
#endif

#ifndef BCC_BLOCK_IGNORE
	topOffset = src->stack + src->stackSize - (unsigned*) MK_FP( helpSs, helpSp);		// offset from the bottom of the src stack to the top
#endif

#ifndef BCC_BLOCK_IGNORE
	dst->ss = FP_SEG(dst->stack + dst->stackSize - topOffset);							// use the calculated offset to calculate the initial values of the childs registers
	dst->sp = FP_OFF(dst->stack + dst->stackSize - topOffset);
	dst->bp = FP_OFF(dst->stack + dst->stackSize - topOffset);
#endif

#ifndef BCC_BLOCK_IGNORE
	srcBottomOffset = FP_OFF(src->stack + src->stackSize);								// offset part of the bottom of the src and dst stacks, used to update bp values
	dstBottomOffset = FP_OFF(dst->stack + dst->stackSize);
#endif

//	dst->stack[dst->stackSize - topOffset + 2] = 0;										// first solution was to change an automatic variable on the stack that was returned

	curBp = src->stack[src->stackSize - topOffset];										// bp value is on top of the stack

	while (curBp != 0){																	// 0 does not need to be updated, it is already on the dst stack

		dst->stack[dst->stackSize - topOffset] = dstBottomOffset - (srcBottomOffset - curBp);		// calculate child's bp value relative to dst stack

#ifndef BCC_BLOCK_IGNORE
		topOffset =  src->stack + src->stackSize - (unsigned*) MK_FP(helpSs, curBp);				// get the offset of the next bp on the parent stack
#endif

		curBp = src->stack[src->stackSize - topOffset];												// read the new bp value

	}

	dst->status = READY;
	((PCBList*)SysUtil::activeThreads)->add(dst);				// child thread is now ready to be run, add it to the list of active threads and to the scheduler
	Scheduler::put(dst);

}

Thread* cloneWrapper() {										// used only to record the new thread in the toBeDeleted list
	Thread* temp = PCB::running->myThread->clone();
	toBeDeleted.add(temp);
	return temp;
}

ID Thread::fork() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//	ID returnValue = -1;
	Thread* newThread = cloneWrapper();

	src = (PCB*)PCB::running;
	dst = newThread->myPCB;

	src->myChildrenList->add(dst);

//	returnValue = dst->myID;

	splitStack();

	if (PCB::running->myID == src->myID) {
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
		return dst->myID;
	}
	else {
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
		return 0;
	}
//	return returnValue;
}

Thread* Thread::clone() const{
	return new Thread(this->myPCB->stackSize*sizeof(unsigned), this->myPCB->timeSlice);
}

void Thread::exit() {
	PCB::exit();
}
void Thread::waitForForkChildren() {
	PCB::waitForForkChildren();
}


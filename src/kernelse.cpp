//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		kernelse.cpp
//	Contains: 	KernelSem class implementation
//

#include "kernelse.h"
#include "SCHEDULE.H"
#include "sysutil.h"
#include <IOSTREAM.H>

																			// interrupts are disabled in every method because data of the KernelSem
																			// is modified by the newRoutine

	KernelSem::KernelSem(int init){
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif
		value = init;														// initialize value and both waiting lists
		unlimitedWaitList = new PCBList();
		timeLimitedList = new PCBTimedList();

		((SemList*)SysUtil::allSemaphores)->add(this);						// add this KernelSem to the global list
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

	}

	KernelSem::~KernelSem(){
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

		PCB* temp;

		while (!(timeLimitedList->isEmpty())) {								// release all threads waiting on this semaphore
			value++;
			temp = timeLimitedList->remove();
			temp->status = READY;
			temp->kernelSemRetVal = 0;										// not unblocked by a signal
			Scheduler::put(temp);
		}

		delete timeLimitedList;												// delete dynamically allocated list

		while (!(unlimitedWaitList->isEmpty())) {
			value++;
			temp = unlimitedWaitList->remove();
			temp->status = READY;
			temp->kernelSemRetVal = 0;										// not unblocked by a signal
			Scheduler::put(temp);
		}

		delete unlimitedWaitList;											// delete dynamically allocated list

		((SemList*)SysUtil::allSemaphores)->remove(this);					// update the global list of semaphores

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

	}

	int KernelSem::wait(Time maxTimeToWait) {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif
		if (--value < 0) {

			if ( maxTimeToWait == 0) {														// if the thread is blocked indefinitely


				//				cout << "Thread " << ((PCB*)PCB::running)->getId() << " was blocked indefinitely! SEMVAL = " << value << endl;
				//				asm cli;

				PCB::running->status = BLOCKED;
				unlimitedWaitList->add((PCB*)PCB::running);									// add running to the list of threads waiting for a signal

				lock temp = SysUtil::globalLock;											// transfer the value of the globalLock for later
				SysUtil::globalLock = 0;													// reseting the globalLock so context switch can occur

				PCB::dispatch();															// switch context

				SysUtil::globalLock = temp;													// returning the globalLock to its original value

			}
			else {

				//				cout << "Thread " << ((PCB*)PCB::running)->getId() << " was blocked for " << maxTimeToWait << " time increments! SEMVAL = " << value << endl;
				//				asm cli;

				PCB::running->status = BLOCKED;
				timeLimitedList->add((PCB*)PCB::running, maxTimeToWait);					// add running to the list of threads waiting for a signal or the timer to unblock them

				lock temp = SysUtil::globalLock;											// transfer the value of the globalLock for later
				SysUtil::globalLock = 0;													// reseting the globalLock so context switch can occur

				PCB::dispatch();															// switch context

				SysUtil::globalLock = temp;													// returning the globalLock to its original value

			}

			//			if (PCB::running->kernelSemRetVal == 0) {
			//				cout << "Thread " << ((PCB*)PCB::running)->getId() << " ran out of time on the semaphore! SEMVAL = " << value << endl;
			//			} else {
			//				cout << "Thread " << ((PCB*)PCB::running)->getId() << " was unblocked by a signal! SEMVAL = " << value << endl;
			//		}

		}
		else {
			PCB::running->kernelSemRetVal = 0;												// if the thread does not have to wait, return 0
		}

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

		return PCB::running->kernelSemRetVal;

	}

	void KernelSem::signal() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

		PCB* temp;

		if (value++ < 0) {
			if (!(timeLimitedList->isEmpty())) {											// unblock the threads waiting for a limited amount of time first
				temp = timeLimitedList->remove();
				temp->status = READY;
				temp->kernelSemRetVal = 1;													// unblocked by a signal, return 1
				Scheduler::put(temp);
			}
			else {																			// if there are no threads in the first list, unblock a thread from the other list
				temp = unlimitedWaitList->remove();
				temp->status = READY;
				temp->kernelSemRetVal = 1;													// unblocked by a signal, return 1
				Scheduler::put(temp);
			}
		}
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

	}

	void KernelSem::semTick() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//		cout << "Tick " << timeLimitedList->head->timeToWait << endl;

		PCB* temp;

		if (!(timeLimitedList->isEmpty()) && (--timeLimitedList->head->timeToWait == 0)) {			// if the list is not empty and the head of the list ran out of time
																									// time to wait is updated in the condition
			do {
				value++;																			// update the value since one thread is being unblocked
				temp = timeLimitedList->remove();
				temp->status = READY;
				temp->kernelSemRetVal = 0;															// not unblocked by a signal, return 0
				Scheduler::put(temp);

			} while (!(timeLimitedList->isEmpty()) && (timeLimitedList->head->timeToWait == 0));	// repeat this if there are more threads waiting for the same amount of time
		}
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

	}

	int KernelSem::val() const {
		return value;
	}

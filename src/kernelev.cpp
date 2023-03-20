//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		kernelev.cpp
//	Contains: 	KernelEv class implementation
//


#include "kernelev.h"
#include <IOSTREAM.H>

	KernelEv::KernelEv( IVTNo myNo ) {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//		cout << "KernelEv constructor" << endl;
//		asm cli;

		creator = (PCB*)PCB::running;										// only the thread that created the event can be blocked on it
		blocked = 0;
		myEntry = myNo;														// number of entry in the IVT

		IVTEntry::arrIVTEntry[myEntry]->addListener(this);					// connect with the object of IVTEntry which is associated with the same entry

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

	KernelEv::~KernelEv() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//		cout << "KernelEv destructor." << endl;
//		asm cli;

//		creator = (PCB*)PCB::running;
//		blocked = 0;

		if (blocked == 1) {																// if there is a thread blocked on the event, unblock it

			blocked = 0;

			creator->status = READY;
			Scheduler::put(creator);
		}

		IVTEntry::arrIVTEntry[myEntry]->removeListener();								// disconnect from the IVTEntry object to avoid accessing a destroyed object

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

	void KernelEv::wait() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif
		if (PCB::running == creator && blocked == 0) {									// if the calling thread is the same one that created the event and there are no
																						// threads waiting for the event (which should not ever happen) block the running thread

//			cout << "Thread blocked." << endl;
//			asm cli;

			blocked = 1;																// flag that there is a thread waiting for the event

			creator->status = BLOCKED;

			lock temp = SysUtil::globalLock;											// transfer the value of the globalLock for later
			SysUtil::globalLock = 0;													// reset the globalLock so context switch can occur

			PCB::dispatch();															// switch context

			SysUtil::globalLock = temp;													// return the globalLock to its original value


		}
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

	void KernelEv::signal() {															// called due to an asynchronous interrupt
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif
		if (blocked == 1) {

//			cout << "Thread unblocked by a signal." << endl;
//			asm cli;

			blocked = 0;																// flag that no threads are waiting for the event now

			creator->status = READY;													// unblock the thread to which this event belongs
			Scheduler::put(creator);

			PCB::dispatch();															// switch context, lock should not be unlocked since the running thread may be in
																						// a critical section

		}

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

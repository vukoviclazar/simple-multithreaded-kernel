//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		ivtentry.cpp
//	Contains: 	IVTEntry class implementation
//

#include "ivtentry.h"
#include "sysutil.h"
#include <DOS.H>
#include <IOSTREAM.H>

IVTEntry* IVTEntry::arrIVTEntry[256];							// global list of IVTEntry pointers, one for each entry in the interrupt vector table,
																// used for linking with instances of KernelEv class

	IVTEntry::IVTEntry( IVTNo myNo, pInterrupt mypInterrupt) {

#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//		cout << "IVTEntry constructor." << endl;
//		asm cli;

		myEntryNumber = myNo;									// save IVT entry number and both old and new routines
		myNewRoutine = mypInterrupt;
		myEvent = 0;

		initializeIVT();										// redirect the interrupt routine to call myNewRoutine, and save the old one to myOldRoutine

		arrIVTEntry[myEntryNumber] = this;						// add this object to the global list

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

	}

	IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//		cout << "IVTEntry destructor." << endl;
//		asm cli;

		arrIVTEntry[myEntryNumber] = 0;							// set global pointer to null

		restoreIVT();											// restore the initial state of the IVT

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

	void IVTEntry::addListener(KernelEv* myEv) {				// link a KernelEv object to this IVTEntry object, this has to be done in order to signal the KernelEv
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//		cout << "Listener added." << endl;
//		asm cli;

		myEvent = myEv;
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

	}

	void IVTEntry::removeListener() {							// reset myEvent pointer to avoid accessing dead objects
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif
		myEvent = 0;
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}

	void IVTEntry::sendSignal() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

		if (myEvent != 0) {										// if there is an event linked to this IVTEntry, signal it

//			cout << "Signaling my event." << endl;
//			asm cli;

			myEvent->signal();
		}
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif

	}

	void IVTEntry::callOldRoutine() {							// call the old interrupt routine
#ifndef BCC_BLOCK_IGNORE
		hard_lock
#endif

//		cout << "Calling old routine." << endl;
//		asm cli;

		myOldRoutine();

#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
	}


	void IVTEntry::initializeIVT() {							// redirect the interrupt routine to call myNewRoutine, and save the old one to myOldRoutine
#ifndef BCC_BLOCK_IGNORE
		hard_lock

		myOldRoutine = getvect(myEntryNumber);
		setvect(myEntryNumber, myNewRoutine);

		hard_unlock
#endif
	}
	void IVTEntry::restoreIVT() {
#ifndef BCC_BLOCK_IGNORE
		hard_lock

		setvect(myEntryNumber, myOldRoutine);

		hard_unlock
#endif
	}

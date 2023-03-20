//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		event.cpp
//	Contains: 	Event class implementation
//

#include "event.h"
#include "kernelev.h"

														// all methods are wrappers for the methods of KernelEv

	Event::Event(IVTNo ivtNo) {
#ifndef BCC_BLOCK_IGNORE
		soft_lock
#endif
		myImpl = new KernelEv(ivtNo);
#ifndef BCC_BLOCK_IGNORE
		soft_unlock
#endif
	}

	Event::~Event() {
#ifndef BCC_BLOCK_IGNORE
		soft_lock
#endif
		delete myImpl;
#ifndef BCC_BLOCK_IGNORE
		soft_unlock
#endif
	}

	void Event::wait() {
		myImpl->wait();
	}

	void Event::signal() {
		myImpl->signal();
	}

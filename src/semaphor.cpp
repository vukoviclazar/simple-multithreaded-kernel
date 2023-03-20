//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		semaphor.cpp
//	Contains: 	Semaphore class implementation
//

#include "semaphor.h"
#include "sysutil.h"
#include "kernelse.h"

														// all methods are defined as wrappers for respective methods of the KernelSem class

Semaphore::Semaphore(int init) {
#ifndef BCC_BLOCK_IGNORE								// locked context because of operator new
	soft_lock
#endif
	myImpl = new KernelSem(init);
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

Semaphore::~Semaphore() {								// locked context because of operator delete
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif
	delete myImpl;
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

int Semaphore::wait(Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}
void Semaphore::signal() {
	myImpl->signal();
}

int Semaphore::val() const {
	return myImpl->val();
}

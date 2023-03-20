//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		kernelev.h
//	Contains: 	KernelEv class interface
//


#ifndef _KERNELEV_H_
#define _KERNELEV_H_

#include "event.h"
#include "pcb.h"
#include "ivtentry.h"
#include "SCHEDULE.H"

class KernelEv {

public:

	KernelEv( IVTNo );

	~KernelEv();

	void wait();

	void signal();

private:

	PCB* creator;

	IVTNo myEntry;

	char blocked;


};

#endif /* _KERNELEV_H_ */

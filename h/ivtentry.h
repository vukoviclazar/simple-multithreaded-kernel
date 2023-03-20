//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		ivtentry.h
//	Contains: 	IVTEntry class interface
//

#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "sysutil.h"
#include "kernelev.h"

typedef void interrupt (*pInterrupt)(...);

#define PREPAREENTRY( number, callOldFlag )\
	void interrupt newRoutineNo##number(...) { \
		if (callOldFlag != 0) { \
			IVTEntry::arrIVTEntry[number]->callOldRoutine();\
		}\
		IVTEntry::arrIVTEntry[number]->sendSignal();\
	}\
	IVTEntry newEntryNo##number ( number, newRoutineNo##number ); \



class IVTEntry {

public:

	IVTEntry( IVTNo, pInterrupt );

	~IVTEntry();

	void addListener(KernelEv*);
	void removeListener();

	void callOldRoutine();

	void sendSignal();

	static IVTEntry* arrIVTEntry[256];

private:

	void initializeIVT();
	void restoreIVT();

	KernelEv* myEvent;
	pInterrupt myOldRoutine;
	pInterrupt myNewRoutine;
	IVTNo myEntryNumber;


};

#endif /* _IVTENTRY_H_ */

//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		event.h
//	Contains: 	Event class interface
//


#ifndef _EVENT_H_
#define _EVENT_H_

typedef unsigned char IVTNo;
class KernelEv;

class Event {

public:

	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:

	friend class KernelEv;

	void signal(); // can call KernelEv

private:

	KernelEv* myImpl;

};

#endif /* _EVENT_H_ */

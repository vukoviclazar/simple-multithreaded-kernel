//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		pcbtlist.h
//	Contains: 	PCBTimedList class interface, used for implementation of KernelSem
//

#ifndef _PCBTLIST_H_
#define _PCBTLIST_H_

typedef unsigned int Time;

class PCB;

struct PCBTimedNode {														// list node struct for PCB, there is a field for time left used for ordered insertion into the list
	PCB* data;
	PCBTimedNode* next;
	Time timeToWait;

	PCBTimedNode(PCB* data, PCBTimedNode* next = 0, Time timeToWait = 0) {
		this->data = data;
		this->next = next;
		this->timeToWait = timeToWait;
	}
};

class PCBTimedList {														// linked list of PCBs, ordered by least number of time increments to wait
																			// the first element of the list contains its actual waiting time left, the others
																			// contain the difference between their waiting time and the waiting time of the element
																			// before them, so only the head of the list needs to be updated each time the timer ticks

public:

	PCBTimedList();
	~PCBTimedList();

	void add(PCB* newPCB, Time maxTimeToWait);								// inserts the element in its place according to the maxTimeToWait argument

	PCB* remove();															// removes an element from the beginning of the list

	int isEmpty();

	PCBTimedNode* head;

protected:

	int length;

private:


};

#endif /* _PCBTLIST_H_ */

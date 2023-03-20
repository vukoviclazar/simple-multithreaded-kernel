//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		pcblist.h
//	Contains: 	PCBList class interface, a linked list of PCBs
//


#ifndef _PCBLIST_H_
#define _PCBLIST_H_

#include "pcb.h"
#include "sysutil.h"

struct PCBNode {									// list node struct for PCB, there is a field for the next element for linking
	PCB* data;
	PCBNode* next;

	PCBNode(PCB* data, PCBNode* next = 0) {
		this->data = data;
		this->next = next;
	}

};


class PCBList {										// FIFO linked list of PCBs

public:

	PCBList();

	void add(PCB*);									// insert an element at the end of the list

	PCB* remove();									// remove an element from the beginning of the list
	PCB* removeByID(ID id);							// remove an element with a given ID number from the list

	PCB* getByID(ID id);							// get a pointer to the PCB that has a given ID number from the list

	int isEmpty();

	~PCBList();

private:

	PCBNode* head, * tail;							// first and last element of the list

	int length;										// number of elements in the list

};

#endif /* _PCBLIST_H_ */

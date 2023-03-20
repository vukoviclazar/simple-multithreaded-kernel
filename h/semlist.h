//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		semlist.h
//	Contains: 	SemList class interface, a linked list of KernelSems
//

#ifndef _SEMLIST_H_
#define _SEMLIST_H_

#include "kernelse.h"

class KernelSem;

struct SemNode {										// list node struct for KernelSem, there is a field for the next element for linking
	KernelSem* data;
	SemNode* next;

	SemNode (KernelSem* data, SemNode* next = 0) {
		this->data = data;
		this->next = next;
	}

};

class SemList {

public:

	SemList();

	void add(KernelSem*);								// insert an element at the start of the list

	KernelSem* remove(KernelSem*);						// remove a given element from the list

	void tickAll();										// call tick method for all KernelSems in the list, used in the new interrupt routine

	int isEmpty();

	~SemList();

private:

	SemNode* head;										// first and last element of the list

	int length;											// number of elements in the list

};

#endif /* SEMLIST_H_ */

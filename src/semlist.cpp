//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		semlist.cpp
//	Contains: 	SemList class implementation, a linked list of KernelSems
//

#include "semlist.h"

SemList::SemList(): head(0), length(0) {}										// ** interupts are disabled in all methods because tickAll is called from the interrupt routine

SemList::~SemList() {
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif
	SemNode* current = head;
	SemNode* old = 0;
	while (current != 0) {														// iterate through the list and delete every element
		old = current;
		current= current->next;
		delete old;
	}
	head = 0;
	length = 0;
#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif
}

void SemList::add(KernelSem* newSem) {
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif
	head = new SemNode(newSem, head);
	length++;																	// one more element is inserted
#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif
}

KernelSem* SemList::remove(KernelSem* target) {
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif
	SemNode* temp = head;
	SemNode* prev = 0;

	while (temp != 0) {
		if (temp->data == target) {
			break;
		}
		prev = temp;
		temp = temp->next;
	}

	if (temp == 0) {															// if target is not found
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
		return 0;
	} else {																	// if target is found
		if (temp == head) {
			head = head->next;													// update head if removing the first element
		} else {
			prev->next = temp->next;											// update pointers
		}
		delete temp;
#ifndef BCC_BLOCK_IGNORE
		hard_unlock
#endif
		return target;
	}

}
void SemList::tickAll() {
#ifndef BCC_BLOCK_IGNORE
	hard_lock
#endif
	for (SemNode* current = head; current != 0; current = current->next) {
		current->data->semTick();												// call semTick for every KernelSem in the list, this method will be called from newRoutine
	}
#ifndef BCC_BLOCK_IGNORE
	hard_unlock
#endif
}

int SemList::isEmpty() {
	return (length == 0);
}


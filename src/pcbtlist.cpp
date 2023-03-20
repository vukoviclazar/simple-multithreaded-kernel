//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		pcbtlist.cpp
//	Contains: 	PCBTimedList class implementation, used for implementation of KernelSem
//

#include "pcbtlist.h"
#include "sysutil.h"


	PCBTimedList::PCBTimedList(): length(0), head(0) {}

	PCBTimedList::~PCBTimedList(){
#ifndef BCC_BLOCK_IGNORE														// locked context because of the operator delete
	soft_lock
#endif
	PCBTimedNode* current = head;
	PCBTimedNode* old = head;
	while (current != 0) {														// iterate through the list and delete every element
		old = current;
		current= current->next;
		delete old;
	}
	head = 0;
	length = 0;
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
	}

	void PCBTimedList::add(PCB* newPCB, Time maxTimeToWait) {
#ifndef BCC_BLOCK_IGNORE														// locked context because of operator new
		soft_lock
#endif

	if (head == 0) {															// if the list is empty, head is the new element
		head = new PCBTimedNode(newPCB, 0, maxTimeToWait);
	}
	else {
		PCBTimedNode* prev = head;
		PCBTimedNode* current = head;
		while ((current != 0) && (current->timeToWait <= maxTimeToWait)) {		// iterate through the list and find the place of the new element, operator <= to keep FIFO order
			maxTimeToWait -= current->timeToWait;								// update the waiting time
			prev = current;
			current = current->next;
		}
		if (current == head) {													// if the element is being inserted at the start, update the head pointer
			head = new PCBTimedNode(newPCB, current, maxTimeToWait);
		}
		else {																	// otherwise, update the previous element's next pointer
			prev->next = new PCBTimedNode(newPCB, current, maxTimeToWait);
		}
		if (current != 0) {														// if there are elements after the new, update the next's time to wait
			current->timeToWait -= maxTimeToWait;
//			current = current->next;
		}
	}
	length++;

#ifndef BCC_BLOCK_IGNORE
		soft_unlock
#endif
	}

	PCB* PCBTimedList::remove() {
#ifndef BCC_BLOCK_IGNORE														// locked context because of operator delete
		soft_lock
#endif
		PCBTimedNode* temp = head;
		PCB* retVal;
		if (head != 0) {
			head = head->next;
			if (head != 0) {
				head->timeToWait += temp->timeToWait;							// update the new head element's time to wait
			}
			retVal = temp->data;
			delete temp;
			length--;
		}
		else {
			retVal = 0;
		}
#ifndef BCC_BLOCK_IGNORE
		soft_unlock
#endif
		return retVal;
	}

int PCBTimedList::isEmpty() {
	return (length == 0);
}

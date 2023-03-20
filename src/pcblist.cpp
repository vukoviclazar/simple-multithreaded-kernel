//
//	Operating systems 1 project
//
//	Author: 	Lazar Vukovic
//	File: 		pcblist.cpp
//	Contains: 	PCBList class implementation, a linked list of PCBs
//

#include "pcblist.h"
#include <IOSTREAM.H>

PCBList::PCBList(): head(0), tail(0), length(0) {} 								// the list is just created and empty

PCBList::~PCBList() {															// lock context because of the operator delete
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif
	PCBNode* current = head;
	PCBNode* old = 0;
	while (current != 0) {														// iterate through the list and delete every element
		old = current;
		current= current->next;
		delete old;
	}
	head = tail = 0;
	length = 0;
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

void PCBList::add(PCB* newPCB) {
#ifndef BCC_BLOCK_IGNORE									// lock context because of the operator new
	soft_lock
#endif
	if (head == 0) {										// if the list is empty, the element being inserted becomes both the first and the last
		head = tail = new PCBNode(newPCB);
	} else {
		tail->next = new PCBNode(newPCB);					// if the list is not empty, insert element after the last, updating all pointers
		tail = tail->next;									// now tail is the inserted element
	}
	length++;												// one more element is inserted
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
}

PCB* PCBList::remove() {
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif
	if (head == 0) {										// the list is empty, return null
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
		return 0;
	}
	else {
		PCBNode* temp = head;
		PCB* retVal = head->data;
		head = head->next;
		if (head == 0) {									// if there are no elements left, head and tail should be null
			tail = 0;
		}
		length--;											// one element is removed
		delete temp;										// PCBNode was dynamically allocated so it is deleted
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
		return retVal;
	}

}


PCB* PCBList::removeByID(ID id) {
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif

	PCB* retVal = 0;

	PCBNode* temp = head;
	PCBNode* prev = 0;

	while (temp != 0) {															// iterate through the list
		if (temp->data->getId() == id) {										// if the element is found, set helper pointers and break the loop
			retVal = temp->data;
			break;
		}
		prev = temp;
		temp= temp->next;
	}

	if (temp != 0) {															// if the element is found
		if (temp == head) {														// if the element is the first element, update head pointer
			head = head->next;
			if (head == 0) {													// if there are no elements left, head and tail should be null
				tail = 0;
			}
		}
		else if (temp == tail) {												// if the element is the last element, update tail pointer
			tail = prev;														// there is no need for checking the case no elements are left, it is covered
			tail->next = 0;
		}
		else {																	// if the element is neither the first nor the last in the list
			prev->next = temp->next;											// update the pointers
		}
		length--;
		delete temp;
	}
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif
	return retVal;
}

PCB* PCBList::getByID(ID id) {
#ifndef BCC_BLOCK_IGNORE
	soft_lock
#endif
	PCB* retVal = 0;
	for (PCBNode* current = head; current != 0; current = current->next) {		// iterate through the list
//		cout << current->data->getId() << " ";
		if (current->data->getId() == id) {										// if the element is found, set the helper pointer and break the loop
			retVal = current->data;
			break;
		}
	}
//	cout << endl;
#ifndef BCC_BLOCK_IGNORE
	soft_unlock
#endif																			// what if there is a dispatch here and the thread is destroyed by the time calling thread gets
	return retVal;																// the cpu again???
}

int PCBList::isEmpty() {
	return (length == 0);														// same question, should be called from a protected section
}


//void PCBList::addFront(PCB* newPCB) {
//#ifndef BCC_BLOCK_IGNORE
//	soft_lock
//#endif
//	if (head == 0) {										// if the list is empty, the element being inserted becomes both the first and the last
//		head = tail = new PCBNode(newPCB);
//	} else {
//		head->prev = new PCBNode(newPCB, head, 0);			// if the list is not empty, insert element before the first, updating all pointers
//		head = head->prev;									// now head is the inserted element
//	}
//	length++;												// one more element is inserted
//#ifndef BCC_BLOCK_IGNORE
//	soft_unlock
//#endif
//}

//PCB* PCBList::removeBack() {
//#ifndef BCC_BLOCK_IGNORE
//	soft_lock
//#endif
//	if (head == 0) {										// the list is empty, return null
//#ifndef BCC_BLOCK_IGNORE
//	soft_unlock
//#endif
//		return 0;
//
//	}
//	else {
//		PCBNode* temp = tail;
//		PCB* retVal = tail->data;
//		tail = tail->prev;
//		if (tail != 0) {									// if there are elements left, update the next pointer of the new tail
//			tail->next = 0;
//		}
//		else {												// if there are no elements left, head and tail should be null
//			head = 0;
//		}
//		length--;											// one element is removed
//		delete temp;										// PCBNode was dynamically allocated so it is deleted
//#ifndef BCC_BLOCK_IGNORE
//	soft_unlock
//#endif
//		return retVal;
//	}
//}

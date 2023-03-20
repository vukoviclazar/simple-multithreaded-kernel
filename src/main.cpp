//
//	Operating systems 1 project
//
//	Author:		Lazar Vukovic
//	File: 		main.cpp
//	Contains: 	main function and UserMain class, which is used for creating a thread with userMain function as its body
//

#include "sysutil.h"
#include <iostream.h>

int userMain(int , char** );															// declaration of userMain function

class UserMainThread: public Thread {													// UserMainThread is a thread which runs a function that satisfies the userMain interface
public:																					// and stores its return value in one of its fields

	UserMainThread(int argc, char** argv, int (*body)(int , char* [])): Thread(), argc(argc), argv(argv), body(body), returnValue(0) {}
	virtual ~UserMainThread() {	waitToComplete();}
	Thread* clone() const {	return new UserMainThread(argc, argv, body);}

	int argc;																			// argc of userMain thread
	char** argv;																		// argv of userMain thread
	volatile int returnValue;															// used for storing the value returned from userMain

	int (*body)(int , char* []);														// pointer to the userMain function

protected:
	virtual void run() {
		returnValue = body(argc, argv);
	}
};

int main(int argc, char* argv[]) {

	SysUtil::initializeIVT();

//	cout << "Pre poziva new" << endl;

	UserMainThread* userm = new UserMainThread(argc, argv, userMain);					// start a new thread that will run the userMain function
	userm->start();
	dispatch();																			// switch context to the UserMain thread

	int returnValue = userm->returnValue;												// transfer the return value before deleting

	delete userm;

//	cout << "End of main, return value is " << returnValue << endl;

	SysUtil::restoreIVT();

	return returnValue;

}



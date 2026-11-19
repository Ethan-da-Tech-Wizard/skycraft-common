// ==============================================================================
// Program: IThread.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of thread creation, execution dispatch, and thread joining.
//          Inputs: Thread parameter payloads.
//          Outputs: Active background thread executing target static method.
//          Algorithm: Win32 CreateThread invocation with static wrapper trampoline.
// ==============================================================================

#include "IThread.h"

IThread::IThread()
{
	mainProc = NULL;
	mainProcParam = NULL;
	stopRequested = false;
	isRunning = false;
	theThread = NULL;
	threadID = 0;
}

// The destructor kills a still-running thread (ForceStop) and closes its handle,
// so a thread should be stopped cooperatively before the object is destroyed.
IThread::~IThread()
{
	ForceStop();

	if(theThread)
	{
		CloseHandle(theThread);
	}
}

// Create a Win32 thread running _ThreadProc, which calls the supplied function.
// Does nothing if already running.
void IThread::Start(MainProcPtr proc, void * procParam)
{
	if(!isRunning)
	{
		isRunning = true;
		stopRequested = false;

		mainProc = proc;
		mainProcParam = procParam;

		theThread = CreateThread(NULL, 0, _ThreadProc, static_cast<IThread *>(this), 0, &threadID);
	}
}

// Cooperative stop: only sets a flag (stopRequested) that the thread's
// function is expected to poll; it does not interrupt the thread.
void IThread::Stop(void)
{
	if(isRunning)
	{
		stopRequested = true;
	}
}

// TerminateThread is a last resort: it does not run cleanup code and can leave
// locks held or memory leaked.
void IThread::ForceStop(void)
{
	if(isRunning)
	{
		TerminateThread(theThread, 0);

		isRunning = false;
	}
}

// Trampoline: Win32 needs a plain function, so the IThread pointer is passed as
// the parameter and the user's callback is invoked from here. isRunning is
// cleared when the callback returns.
UInt32 IThread::_ThreadProc(void * param)
{
	IThread	* _this = (IThread *)param;

	if(_this->mainProc)
		_this->mainProc(_this->mainProcParam);

	_this->isRunning = false;

	return 0;
}

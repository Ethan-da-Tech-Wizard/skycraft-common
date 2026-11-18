// ==============================================================================
// Program: IEvent.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of Win32 Event handle allocation, signaling, and timeouts.
//          Inputs: Wait timeout durations in milliseconds.
//          Outputs: Wait result statuses (WAIT_OBJECT_0, WAIT_TIMEOUT, WAIT_FAILED).
//          Algorithm: Win32 WaitForSingleObject synchronization dispatch.
// ==============================================================================

#include "IEvent.h"

// ---------------------------------------------------------------------------
// Constructor: Invokes Win32 CreateEvent with specified reset mode
// ---------------------------------------------------------------------------
IEvent::IEvent()
{
	theEvent = CreateEvent(NULL, true, true, NULL);
	ASSERT(theEvent);

	blockCount.Set(0);
}

IEvent::~IEvent()
{
	CloseHandle(theEvent);
}

bool IEvent::Block(void)
{
	if(blockCount.Increment() == 1)
		return (ResetEvent(theEvent) != 0);
	else
		return true;
}

bool IEvent::UnBlock(void)
{
	if(blockCount.Decrement() == 0)
		return (SetEvent(theEvent) != 0);
	else
		return true;
}

bool IEvent::Wait(UInt32 timeout)
{
	switch(WaitForSingleObject(theEvent, timeout))
	{
		case WAIT_ABANDONED:
			HALT("IEvent::Wait: got abandoned event");
			return false;

		case WAIT_OBJECT_0:
			return true;

		default:
		case WAIT_TIMEOUT:
			gLog.FormattedMessage("IEvent::Wait: timeout");
			return false;
	}
}

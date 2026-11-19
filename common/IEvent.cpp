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

/*
 * IEvent is a "gate": waiters pass while it is open and block while it is closed.
 * It wraps a Win32 manual-reset event (stays signalled until reset), plus a counter so
 * several independent callers can Block() it and it only reopens when each has UnBlock()ed.
 */

/// Create the underlying event as manual-reset (2nd arg true) and initially signalled/open (3rd arg true).
// ---------------------------------------------------------------------------
// Constructor: Invokes Win32 CreateEvent with specified reset mode
// ---------------------------------------------------------------------------
// Failure to create an event is not recoverable here, so abort.
IEvent::IEvent()
{
	theEvent = CreateEvent(NULL, true, true, NULL);
	ASSERT(theEvent);

	blockCount.Set(0);
}

/// Release the Win32 handle.
IEvent::~IEvent()
{
	CloseHandle(theEvent);
}

/**
 * Close the gate. Calls nest: only the first Block() actually resets the event.
 *
 * @return true on success (or when it was already blocked).
 */
bool IEvent::Block(void)
{
	if(blockCount.Increment() == 1)
		return (ResetEvent(theEvent) != 0);
	else
		return true;
}

/**
 * Undo one Block(). The gate reopens when the count returns to zero.
 *
 * @return true on success (or when other blockers remain).
 */
bool IEvent::UnBlock(void)
{
	if(blockCount.Decrement() == 0)
		return (SetEvent(theEvent) != 0);
	else
		return true;
}

/**
 * Wait for the gate to open.
 *
 * @param timeout Milliseconds; default IEvent::kDefaultTimeout (10 s).
 * @return true when open; false on timeout (logged). An abandoned wait halts, since it
 *         means the owning thread died and the gate can never be trusted.
 */
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
		// Any unexpected wait result is treated like a timeout.
		case WAIT_TIMEOUT:
			gLog.FormattedMessage("IEvent::Wait: timeout");
			return false;
	}
}

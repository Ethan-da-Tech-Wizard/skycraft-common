// ==============================================================================
// Program: IMutex.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of Win32 mutex creation, acquisition timeout, and release.
//          Inputs: Named strings and millisecond timeout limits.
//          Outputs: Boolean acquisition success indicators.
//          Algorithm: Win32 kernel dispatcher object wait routines.
// ==============================================================================


#include "IMutex.h"

/*
 * IMutex: cross-thread (and cross-process capable) Win32 mutex, heavier than ICriticalSection.
 * NOTE: the mutex is created already OWNED by the constructing thread (second CreateMutex
 * argument is true), so that thread must Release() once (or the mutex stays locked for others).
 */

/// Create an unnamed mutex, initially owned by the calling thread.
// ---------------------------------------------------------------------------
// Constructor: Calls Win32 CreateMutex with optional system name
// ---------------------------------------------------------------------------
IMutex::IMutex()
{
	theMutex = CreateMutex(NULL, true, NULL);
}

/// Release the handle.
IMutex::~IMutex()
{
	CloseHandle(theMutex);
}

/**
 * Wait to acquire the mutex.
 *
 * @param timeout Milliseconds to wait (INFINITE to wait forever).
 * @return true if acquired; false on timeout (logged). An abandoned mutex (previous owner thread died
 *         while holding it) halts because shared state may be inconsistent.
 */
bool IMutex::Wait(UInt32 timeout)
{
	switch(WaitForSingleObject(theMutex, timeout))
	{
		case WAIT_ABANDONED:
			HALT("IMutex::Wait: got abandoned mutex");
			return false;

		case WAIT_OBJECT_0:
			return true;

		// Any other result is treated as a failure to acquire.
		default:
		case WAIT_TIMEOUT:
			gLog.FormattedMessage("IMutex::Wait: timeout");
			return false;
	}
}

/// Release one acquisition of the mutex; ASSERTs if the calling thread does not own it.
void IMutex::Release(void)
{
	ASSERT_STR(ReleaseMutex(theMutex), "IMutex::Release: failed to release mutex");
}

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

// ---------------------------------------------------------------------------
// Constructor: Calls Win32 CreateMutex with optional system name
// ---------------------------------------------------------------------------
IMutex::IMutex()
{
	theMutex = CreateMutex(NULL, true, NULL);
}

IMutex::~IMutex()
{
	CloseHandle(theMutex);
}

bool IMutex::Wait(UInt32 timeout)
{
	switch(WaitForSingleObject(theMutex, timeout))
	{
		case WAIT_ABANDONED:
			HALT("IMutex::Wait: got abandoned mutex");
			return false;

		case WAIT_OBJECT_0:
			return true;

		default:
		case WAIT_TIMEOUT:
			gLog.FormattedMessage("IMutex::Wait: timeout");
			return false;
	}
}

void IMutex::Release(void)
{
	ASSERT_STR(ReleaseMutex(theMutex), "IMutex::Release: failed to release mutex");
}

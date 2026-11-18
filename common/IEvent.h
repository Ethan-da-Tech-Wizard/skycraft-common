// ==============================================================================
// Program: IEvent.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Win32 Event synchronization object encapsulation header.
//          Inputs: Manual/auto reset flags, initial signaled state, and optional names.
//          Outputs: Thread signaling, barrier synchronization, and wait completion.
//          Architecture: Coordinates asynchronous loader injection and game threads.
// ==============================================================================

#pragma once

#include "common/IInterlockedLong.h"

// ---------------------------------------------------------------------------
// IEvent: RAII wrapper around Win32 CreateEvent/SetEvent/ResetEvent
// ---------------------------------------------------------------------------
class IEvent
{
	public:
		static const UInt32 kDefaultTimeout = 1000 * 10;

		IEvent();
		~IEvent();

		bool	Block(void);
		bool	UnBlock(void);
		bool	Wait(UInt32 timeout = kDefaultTimeout);

		bool	IsBlocked(void)	{ return blockCount.Get() > 0; }

	private:
		HANDLE				theEvent;
		IInterlockedLong	blockCount;
};

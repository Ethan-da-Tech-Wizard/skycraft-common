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

/**
 * @class IEvent
 *
 * A gate that threads Wait() on: open lets them through, closed blocks them. Block() and
 * UnBlock() nest (counted), so it reopens only when every Block() has been undone.
 * Wraps a Win32 manual-reset event. Used by IReadWriteLock; Windows only.
 */
// ---------------------------------------------------------------------------
// IEvent: RAII wrapper around Win32 CreateEvent/SetEvent/ResetEvent
// ---------------------------------------------------------------------------
class IEvent
{
	public:
		/// Default Wait() timeout: 10 seconds, in milliseconds.
		static const UInt32 kDefaultTimeout = 1000 * 10;

		IEvent();
		~IEvent();

		/// Close the gate (counted). @return true on success.
		bool	Block(void);
		/// Undo one Block(); opens the gate when the count reaches zero. @return true on success.
		bool	UnBlock(void);
		/// Wait until open or until `timeout` ms pass. @return true if open, false on timeout.
		bool	Wait(UInt32 timeout = kDefaultTimeout);

		/// @return true while at least one Block() is outstanding.
		bool	IsBlocked(void)	{ return blockCount.Get() > 0; }

	private:
		HANDLE				theEvent;
		IInterlockedLong	blockCount;
};

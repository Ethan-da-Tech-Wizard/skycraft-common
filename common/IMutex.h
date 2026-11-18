// ==============================================================================
// Program: IMutex.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Header for Win32 kernel mutex synchronization object wrapper.
//          Inputs: Optional system-wide mutex name and ownership flags.
//          Outputs: Inter-process mutual exclusion across loader and game instance.
//          Architecture: Prevents multiple loader instances from injecting simultaneously.
// ==============================================================================

#pragma once

// ---------------------------------------------------------------------------
// IMutex: Win32 CreateMutex/WaitForSingleObject/ReleaseMutex encapsulation
// ---------------------------------------------------------------------------
class IMutex
{
	public:
		static const UInt32	kDefaultTimeout = 1000 * 10;

		IMutex();
		~IMutex();

		bool	Wait(UInt32 timeout = kDefaultTimeout);
		void	Release(void);

	private:
		HANDLE	theMutex;
};

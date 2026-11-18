// ==============================================================================
// Program: IThread.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Win32 thread management and execution lifecycle wrapper header.
//          Inputs: Thread procedure entrypoints, parameter blocks, and stack sizes.
//          Outputs: Thread handles, thread IDs, and thread termination status.
//          Architecture: Spawns worker threads for asynchronous tasks and pipe listeners.
// ==============================================================================

#pragma once

// TODO: I really don't like the interface for this

// ---------------------------------------------------------------------------
// IThread: RAII wrapper for Win32 CreateThread, ResumeThread, and WaitForSingleObject
// ---------------------------------------------------------------------------
class IThread
{
	public:
		typedef void (* MainProcPtr)(void * param);

				IThread();
				~IThread();

		void	Start(MainProcPtr proc, void * procParam = NULL);
		void	Stop(void);
		void	ForceStop(void);

		bool	IsRunning(void)		{ return isRunning; }
		bool	StopRequested(void)	{ return stopRequested; }

		HANDLE	GetHandle(void)		{ return theThread; }

	protected:
		MainProcPtr		mainProc;
		void			* mainProcParam;
		volatile bool	stopRequested;
		bool			isRunning;
		HANDLE			theThread;
		UInt32			threadID;

	private:
		static UInt32 WINAPI _ThreadProc(void * param);
};

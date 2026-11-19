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

/**
 * @class IThread
 *
 * Small wrapper around a Win32 thread: Start() runs a callback on a new thread, Stop() asks
 * it (cooperatively) to finish, ForceStop() kills it. The callback polls StopRequested().
 * Used for background work such as the pipe/IPC servers. TODO (original author): "I really don't like the interface for this".
 */
// TODO: I really don't like the interface for this

// ---------------------------------------------------------------------------
// IThread: RAII wrapper for Win32 CreateThread, ResumeThread, and WaitForSingleObject
// ---------------------------------------------------------------------------
class IThread
{
	public:
		/// Signature of the function a thread runs.
		typedef void (* MainProcPtr)(void * param);

				IThread();
				~IThread();

		/// Launch the thread running proc(procParam); no-op if already running.
		void	Start(MainProcPtr proc, void * procParam = NULL);
		/// Request a cooperative stop (sets the flag the thread should check).
		void	Stop(void);
		/// Terminate immediately without cleanup; last resort.
		void	ForceStop(void);

		/// @return true while the thread function has not yet returned.
		bool	IsRunning(void)		{ return isRunning; }
		/// @return true after Stop() was called; the thread's function should poll this and return.
		bool	StopRequested(void)	{ return stopRequested; }

		/// @return The Win32 thread handle (NULL before the first Start()).
		HANDLE	GetHandle(void)		{ return theThread; }

	// stopRequested is volatile so the polling thread sees the other thread's write.
	protected:
		MainProcPtr		mainProc;
		void			* mainProcParam;
		volatile bool	stopRequested;
		bool			isRunning;
		HANDLE			theThread;
		UInt32			threadID;

	// Win32 thread entry point (must be a static function with WINAPI calling convention).
	private:
		static UInt32 WINAPI _ThreadProc(void * param);
};

// ==============================================================================
// Program: IReadWriteLock.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Reader-writer multi-access synchronization lock header.
//          Inputs: Shared read acquisition requests and exclusive write requests.
//          Outputs: Concurrent read concurrency with safe exclusive writer serialization.
//          Architecture: High-performance concurrency control for game hook tables.
// ==============================================================================

#pragma once

#include "common/ICriticalSection.h"
#include "common/IEvent.h"
#include "common/IInterlockedLong.h"

/**
 * @class IReadWriteLock
 *
 * Multiple-readers / single-writer lock. Usage: bracket shared access with StartRead()/EndRead()
 * and exclusive access with StartWrite()/EndWrite(). Not recursive. A pending writer holds off new
 * readers (see IReadWriteLock.cpp for the protocol).
 */
// ---------------------------------------------------------------------------
// IReadWriteLock: Slim Reader-Writer (SRW) or event-based multi-reader lock
// ---------------------------------------------------------------------------
class IReadWriteLock
{
	public:
		IReadWriteLock();
		~IReadWriteLock();

		/// Acquire shared access (many readers may hold it together).
		void	StartRead(void);
		/// Release shared access.
		void	EndRead(void);
		/// Acquire exclusive access; waits for all readers and other writers.
		void	StartWrite(void);
		/// Release exclusive access.
		void	EndWrite(void);

	// readBlocker: closed while a writer is pending/active so new readers wait.
	// writeBlocker: closed while any reader is inside so a writer waits.
	// enterBlocker: serialises the arrival of readers vs. a starting writer.
	// writeMutex: one writer at a time. readCount: readers currently inside.
	private:
		IEvent				readBlocker;
		IEvent				writeBlocker;
		ICriticalSection	enterBlocker;
		ICriticalSection	writeMutex;
		IInterlockedLong	readCount;
};

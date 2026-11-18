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

// ---------------------------------------------------------------------------
// IReadWriteLock: Slim Reader-Writer (SRW) or event-based multi-reader lock
// ---------------------------------------------------------------------------
class IReadWriteLock
{
	public:
		IReadWriteLock();
		~IReadWriteLock();

		void	StartRead(void);
		void	EndRead(void);
		void	StartWrite(void);
		void	EndWrite(void);

	private:
		IEvent				readBlocker;
		IEvent				writeBlocker;
		ICriticalSection	enterBlocker;
		ICriticalSection	writeMutex;
		IInterlockedLong	readCount;
};

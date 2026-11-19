// ==============================================================================
// Program: IReadWriteLock.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of reader-writer lock acquisition, reader counting, and writer priority.
//          Inputs: Thread lock requests.
//          Outputs: Synchronized execution guarantees without writer starvation.
//          Algorithm: Atomic reader counter with writer wait event.
// ==============================================================================


#include "IReadWriteLock.h"

/*
 * Reader/writer lock built from events and critical sections (so it works on old Windows
 * without SRW locks). Many readers may hold it together; a writer holds it alone.
 * Writers have priority over NEW readers: once a writer arrives it closes readBlocker,
 * so no new reader can enter, and then waits for the current readers to drain.
 * Not recursive: a thread must not take the lock twice, and read->write upgrade deadlocks.
 */

/// Starts with no readers and both gates open.
IReadWriteLock::IReadWriteLock()
{
	readCount.Set(0);
	readBlocker.UnBlock();
	writeBlocker.UnBlock();
}

/// Nothing to release (events and critical sections clean themselves up).
IReadWriteLock::~IReadWriteLock()
{
	//
}

/**
 * Acquire shared (read) access; blocks while a writer is waiting or active.
 * Must be paired with EndRead().
 */
void IReadWriteLock::StartRead(void)
{
	// enterBlocker serialises arrival so a writer that has closed readBlocker cannot be overtaken
	// by a reader that already passed the gate.
	enterBlocker.Enter();
	// Wait here while a writer is pending or running.
	readBlocker.Wait();
	// First reader in closes writeBlocker, so a writer must wait until the last reader leaves.
	if(readCount.Increment() == 1)
		writeBlocker.Block();
	enterBlocker.Leave();
}

/// Release shared access; the last reader out reopens writeBlocker so a waiting writer can proceed.
void IReadWriteLock::EndRead(void)
{
	if(!readCount.Decrement())
		writeBlocker.UnBlock();
}

/**
 * Acquire exclusive (write) access; blocks until all readers have left and any other writer is done.
 * Must be paired with EndWrite().
 */
void IReadWriteLock::StartWrite(void)
{
	// Only one writer at a time.
	writeMutex.Enter();
	enterBlocker.Enter();
	// Stop new readers from entering...
	readBlocker.Block();
	// ...then wait for the readers already inside to finish.
	writeBlocker.Wait();
	enterBlocker.Leave();
}

/// Release exclusive access: reopen the reader gate first, then let the next writer in.
void IReadWriteLock::EndWrite(void)
{
	readBlocker.UnBlock();
	writeMutex.Leave();
}

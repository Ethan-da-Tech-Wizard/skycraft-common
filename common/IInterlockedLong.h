// ==============================================================================
// Program: IInterlockedLong.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Header specification for atomic hardware interlocked integer operations.
//          Inputs: 32-bit and 64-bit integer values subject to concurrent modification.
//          Outputs: Lock-free atomically incremented/decremented values.
//          Architecture: Ultra-low latency synchronization for SKSE64 reference counting.
// ==============================================================================

#pragma once

/**
 * @struct IInterlockedLong
 *
 * A 32-bit integer whose updates are atomic across threads (Win32 Interlocked* functions), used as a counter
 * or flag shared between threads without a lock. Note `long` is 32 bits on Windows; there is no constructor,
 * so call Set() before use (IReadWriteLock and IEvent do this in their constructors).
 */
struct IInterlockedLong
{
	public:
		/// Atomically add 1; @return the NEW value.
		long	Increment(void)	{ return InterlockedIncrement(&value); }
		/// Atomically subtract 1; @return the NEW value.
		long	Decrement(void)	{ return InterlockedDecrement(&value); }
		/// Plain read of the current value (not an atomic read-modify-write; fine for a snapshot).
		long	Get(void)		{ return value; }
		/// Atomically store `in`; @return the PREVIOUS value.
		long	Set(long in)	{ return InterlockedExchange(&value, in); }
		/// Compare-and-swap: store newValue only if the current value equals expectedOldValue. @return the value that was there before (equal to expectedOldValue means the swap happened).
		long	TrySetIf(long newValue, long expectedOldValue)
								{ return InterlockedCompareExchange(&value, newValue, expectedOldValue); }

		// interlock variable semantics: treat the value as a 0/1 ownership flag.
		// interlock variable semantics
		/// Try to take the flag (0 -> 1); @return true if this caller won it.
		bool	Claim(void)		{ return TrySetIf(1, 0) == 0; }
		/// Give the flag back (1 -> 0); @return true if it was held.
		bool	Release(void)	{ return TrySetIf(0, 1) == 1; }

	private:
		// volatile so reads are not cached in a register between interlocked operations.
		volatile long	value;
};

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

struct IInterlockedLong
{
	public:
		long	Increment(void)	{ return InterlockedIncrement(&value); }
		long	Decrement(void)	{ return InterlockedDecrement(&value); }
		long	Get(void)		{ return value; }
		long	Set(long in)	{ return InterlockedExchange(&value, in); }
		long	TrySetIf(long newValue, long expectedOldValue)
								{ return InterlockedCompareExchange(&value, newValue, expectedOldValue); }

		// interlock variable semantics
		bool	Claim(void)		{ return TrySetIf(1, 0) == 0; }
		bool	Release(void)	{ return TrySetIf(0, 1) == 1; }

	private:
		volatile long	value;
};

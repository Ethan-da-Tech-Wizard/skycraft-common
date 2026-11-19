// ==============================================================================
// Program: IMemPool.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of memory pool free-list management and chunk recycling.
//          Inputs: Allocation requests and released block addresses.
//          Outputs: Recycled block pointers.
//          Algorithm: Singly linked free-list pop/push with zero runtime heap calls.
// ==============================================================================


#include "IMemPool.h"

/**
 * Smoke test for IMemPool, driven by hand (it only logs; nothing asserts).
 *
 * Scenario: a 3-slot pool is filled completely, one slot is freed and reallocated, another
 * is freed, and the free/allocated lists are dumped after every step so a reader can check
 * that freed slots are reused first (LIFO free list) and the allocated list order matches.
 */
void Test_IMemPool(void)
{
	// Capacity 3 so the pool becomes full after exactly three allocations.
	IMemPool <UInt32, 3>	pool;

	_DMESSAGE("main: pool test");
	gLog.Indent();

	_DMESSAGE("start");
	pool.Dump();

	UInt32	* data0, * data1, * data2;

	data0 = pool.Allocate();
	_DMESSAGE("alloc0 = %08X", data0);
	pool.Dump();

	data1 = pool.Allocate();
	_DMESSAGE("alloc1 = %08X", data1);
	pool.Dump();

	data2 = pool.Allocate();
	_DMESSAGE("alloc2 = %08X", data2);
	pool.Dump();

	// Free the first allocation and take another: the same slot should come straight back.
	_DMESSAGE("free0 %08X", data0);
	pool.Free(data0);
	pool.Dump();

	data0 = pool.Allocate();
	_DMESSAGE("alloc0 = %08X", data0);
	pool.Dump();

	// Free a different (non-head) slot to exercise the O(n) unlink path in Free().
	_DMESSAGE("free2 %08X", data2);
	pool.Free(data2);
	pool.Dump();

	_DMESSAGE("done");
	pool.Dump();

	gLog.Outdent();
}

// ==============================================================================
// Program: ITypes.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Out-of-line implementation of Bitstring, the heap-backed bit
//          vector declared in ITypes.h.
//          Inputs: bit counts and bit indexes.
//          Outputs: set/clear/test operations on individual bits.
//          Algorithm: bit i lives in byte (i >> 3) at position (i & 7).
// ==============================================================================
//
// How it fits: ITypes.h declares every type in this family; only Bitstring has
// non-inline code, so this is the only translation unit for it. Indexes are
// range-checked with ASSERT_STR, which calls _AssertionFailed (IErrors.cpp)
// and does not return.
//
// Known limits: the default-constructed object has no storage and an
// uninitialised length; Clear() or any bit access on it is invalid until Alloc().

/** Constructs an empty bitstring with no storage (call Alloc() before use). */
#include "ITypes.h"

Bitstring::Bitstring()
:data(NULL)
{
	
}

/** Constructs a bitstring with room for @p inLength bits (see Alloc()). */
Bitstring::Bitstring(UInt32 inLength)
:data(NULL)
{
	Alloc(inLength);
}

/** Frees the storage. */
Bitstring::~Bitstring()
{
	Dispose();
}

/**
 * Allocates storage for @p inLength bits, replacing any previous storage.
 * The size is rounded up to a whole number of bytes; the new bits are NOT
 * cleared (call Clear()).
 */
void Bitstring::Alloc(UInt32 inLength)
{
	Dispose();

	// Round the bit count up to a multiple of 8, then convert to bytes.
	inLength = (inLength + 7) & ~7;
	length = inLength >> 3;

	data = new UInt8[length];
}

/** Frees the storage array (the pointer is not reset afterwards). */
void Bitstring::Dispose(void)
{
	delete [] data;
}

/** Clears every bit to 0. Requires prior Alloc(). */
void Bitstring::Clear(void)
{
	std::memset(data, 0, length);
}

/** Clears the single bit at @p idx (asserts when idx is past the allocated size). */
void Bitstring::Clear(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::Clear: out of range");

	// Mask with the complement of the bit's position inside its byte.
	data[idx >> 3] &= ~(1 << (idx & 7));
}

/** Sets the bit at @p idx to 1 (asserts on out-of-range). */
void Bitstring::Set(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::Set: out of range");

	data[idx >> 3] |= (1 << (idx & 7));
}

/** @return true if the bit at @p idx is 1 (asserts on out-of-range). */
bool Bitstring::IsSet(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::IsSet: out of range");

	return (data[idx >> 3] & (1 << (idx & 7))) ? true : false;
}

/** @return true if the bit at @p idx is 0 (asserts on out-of-range). */
bool Bitstring::IsClear(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::IsClear: out of range");

	return (data[idx >> 3] & (1 << (idx & 7))) ? false : true;
}

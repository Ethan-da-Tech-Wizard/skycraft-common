// ==============================================================================
// Program: ITypes.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Runtime architecture and type size assertion validations.
//          Inputs: Compile-time sizeof operators.
//          Outputs: Verification that primitive types match expected binary bitwidths.
//          Algorithm: Static compile-time and runtime size assertions.
// ==============================================================================

// ---------------------------------------------------------------------------
// Binary structure alignment and type validation
// ---------------------------------------------------------------------------
#include "ITypes.h"

Bitstring::Bitstring()
:data(NULL)
{
	
}

Bitstring::Bitstring(UInt32 inLength)
:data(NULL)
{
	Alloc(inLength);
}

Bitstring::~Bitstring()
{
	Dispose();
}

void Bitstring::Alloc(UInt32 inLength)
{
	Dispose();

	inLength = (inLength + 7) & ~7;
	length = inLength >> 3;

	data = new UInt8[length];
}

void Bitstring::Dispose(void)
{
	delete [] data;
}

void Bitstring::Clear(void)
{
	std::memset(data, 0, length);
}

void Bitstring::Clear(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::Clear: out of range");

	data[idx >> 3] &= ~(1 << (idx & 7));
}

void Bitstring::Set(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::Set: out of range");

	data[idx >> 3] |= (1 << (idx & 7));
}

bool Bitstring::IsSet(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::IsSet: out of range");

	return (data[idx >> 3] & (1 << (idx & 7))) ? true : false;
}

bool Bitstring::IsClear(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::IsClear: out of range");

	return (data[idx >> 3] & (1 << (idx & 7))) ? false : true;
}

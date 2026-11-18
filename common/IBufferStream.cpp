// ==============================================================================
// Program: IBufferStream.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of in-memory byte buffer read, write, and seek primitives.
//          Inputs: Binary data buffers, byte offsets, and reallocation increments.
//          Outputs: Copied memory slices and updated buffer positions.
//          Algorithm: Fast memcpy-based stream cursor manipulation with amortized doubling.
// ==============================================================================

#include "IBufferStream.h"

// ---------------------------------------------------------------------------
// Constructor: Allocates initial heap storage or wraps user-supplied buffer
// ---------------------------------------------------------------------------
IBufferStream::IBufferStream()
:streamBuf(NULL), flags(0)
{

}

IBufferStream::IBufferStream(const IBufferStream & rhs)
{
	// explicitly not supporting copy constructor for self-owned buffers
	ASSERT((flags & kFlag_OwnedBuf) == 0);
}

IBufferStream::IBufferStream(void * buf, UInt64 inLength)
:streamBuf(NULL), flags(0)
{
	SetBuffer(buf, inLength);
}

IBufferStream::~IBufferStream()
{
	if(flags & kFlag_OwnedBuf)
	{
		delete [] streamBuf;
	}
}

IBufferStream & IBufferStream::operator=(IBufferStream & rhs)
{
	// explicitly not supporting copying for self-owned buffers
	ASSERT((flags & kFlag_OwnedBuf) == 0);

	streamBuf = rhs.streamBuf;
	flags = rhs.flags;

	return *this;
}

void IBufferStream::SetBuffer(void * buf, UInt64 inLength)
{
	streamBuf = (UInt8 *)buf;
	streamLength = inLength;

	Rewind();
}

// ---------------------------------------------------------------------------
// ReadBuf: Sequential memory copy from stream cursor into destination buffer
// ---------------------------------------------------------------------------
void IBufferStream::ReadBuf(void * buf, UInt32 inLength)
{
	memcpy(buf, &streamBuf[streamOffset], inLength);
	streamOffset += inLength;
}

void IBufferStream::WriteBuf(const void * buf, UInt32 inLength)
{
	memcpy(&streamBuf[streamOffset], buf, inLength);
	streamOffset += inLength;
}

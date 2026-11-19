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
/*
 * IBufferStream: an IDataStream over a block of memory. Read/write are plain memcpy at the cursor.
 * It does NOT bounds-check (no end-of-buffer test in ReadBuf/WriteBuf) and never grows the buffer:
 * the caller must make sure reads and writes stay inside [0, streamLength).
 * Ownership: by default the buffer is borrowed; OwnBuffer() makes the stream delete[] it in the destructor.
 */

// ---------------------------------------------------------------------------
/// Empty stream with no buffer; call SetBuffer() before reading or writing.
IBufferStream::IBufferStream()
:streamBuf(NULL), flags(0)
{

}

/// Copy constructor is deliberately limited: it asserts that this object does not own its buffer, because two owners would double-free. NOTE: it does not initialise streamBuf or flags, so the assert reads uninitialised memory; copying is not really usable.
IBufferStream::IBufferStream(const IBufferStream & rhs)
{
	// explicitly not supporting copy constructor for self-owned buffers
	ASSERT((flags & kFlag_OwnedBuf) == 0);
}

/**
 * Wrap an existing memory block (not owned unless OwnBuffer() is called afterwards).
 *
 * @param buf      Start of the memory.
 * @param inLength Size in bytes.
 */
IBufferStream::IBufferStream(void * buf, UInt64 inLength)
:streamBuf(NULL), flags(0)
{
	SetBuffer(buf, inLength);
}

/// Frees the buffer only if OwnBuffer() was called (it must then have been allocated with new[]).
IBufferStream::~IBufferStream()
{
	if(flags & kFlag_OwnedBuf)
	{
		delete [] streamBuf;
	}
}

/// Shallow copy of the buffer pointer and flags; refused for an owning target (ASSERT) to avoid leaking or double-freeing. Note: length and offset are not copied.
IBufferStream & IBufferStream::operator=(IBufferStream & rhs)
{
	// explicitly not supporting copying for self-owned buffers
	ASSERT((flags & kFlag_OwnedBuf) == 0);

	streamBuf = rhs.streamBuf;
	flags = rhs.flags;

	return *this;
}

/**
 * Point the stream at a memory block and rewind to its start.
 *
 * @param buf      Memory to wrap.
 * @param inLength Size in bytes; becomes the stream length.
 * A previously owned buffer is not freed here.
 */
void IBufferStream::SetBuffer(void * buf, UInt64 inLength)
{
	streamBuf = (UInt8 *)buf;
	streamLength = inLength;

	Rewind();
}

// Read: copy from the cursor, then advance. No range check (see the class comment).
// ---------------------------------------------------------------------------
// ReadBuf: Sequential memory copy from stream cursor into destination buffer
// ---------------------------------------------------------------------------
void IBufferStream::ReadBuf(void * buf, UInt32 inLength)
{
	memcpy(buf, &streamBuf[streamOffset], inLength);
	streamOffset += inLength;
}

/**
 * Write inLength bytes at the cursor, then advance. No range check, and streamLength is not
 * extended, so the caller must have sized the buffer for everything it will write.
 */
void IBufferStream::WriteBuf(const void * buf, UInt32 inLength)
{
	memcpy(&streamBuf[streamOffset], buf, inLength);
	streamOffset += inLength;
}

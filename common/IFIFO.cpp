// ==============================================================================
// Program: IFIFO.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of circular ring-buffer FIFO queue push and pop operations.
//          Inputs: Raw data buffers and push/pop count specifications.
//          Outputs: Transferred buffer bytes and wrap-around index updates.
//          Algorithm: Modulo pointer arithmetic with atomic overflow checking.
// ==============================================================================


#include "IFIFO.h"

/*
 * Byte ring buffer: data lives in fifoBuf[], starting at fifoBase and running fifoDataLength
 * bytes, wrapping at the end of the buffer. Push writes at the end of that run, Pop/Peek read
 * from the start. All-or-nothing: an operation either transfers the whole request or nothing.
 * Not thread-safe.
 */

/**
 * @param length Capacity in bytes. Must be > 0: offsets are taken modulo the capacity,
 *               so a zero-size buffer would divide by zero.
 */
// ---------------------------------------------------------------------------
// Constructor: Allocates continuous ring memory buffer and resets head/tail
// ---------------------------------------------------------------------------
IFIFO::IFIFO(UInt32 length)
{
	fifoBuf = new UInt8[length];
	fifoBufSize = length;
	fifoBase = 0;
	fifoDataLength = 0;
}

/// NOTE (found while documenting, unchanged): the buffer is allocated with new[] but released with scalar delete.
IFIFO::~IFIFO()
{
	delete fifoBuf;
}

/**
 * Append bytes to the queue.
 *
 * @param buf    Source data.
 * @param length Number of bytes.
 * @return true if stored; false (nothing written) if fewer than `length` bytes of space remain.
 */
bool IFIFO::Push(UInt8 * buf, UInt32 length)
{
	// would that overflow the buffer?
	// would that overflow the buffer?
	if(length > GetBufferRemain())
		return false;

	UInt32	writeOffset = GetWriteOffset();

	// will this cross the end of the buffer? If so, split into the tail piece and the wrapped-around head piece.
	// will this cross the end of the buffer?
	if(writeOffset + length > fifoBufSize)
	{
		UInt32	segmentLength = fifoBufSize - writeOffset;

		std::memcpy(&fifoBuf[writeOffset], buf, segmentLength);
		std::memcpy(fifoBuf, &buf[segmentLength], length - segmentLength);
	}
	else
	{
		std::memcpy(&fifoBuf[writeOffset], buf, length);
	}

	// update pointers: only the length changes on push; the base moves only when data is popped
	// update pointers
	fifoDataLength += length;

	return true;
}

/**
 * Remove bytes from the front of the queue.
 *
 * @param buf    Destination, at least `length` bytes.
 * @param length Number of bytes.
 * @return true if copied and removed; false (queue unchanged) if fewer than `length` bytes are queued.
 */
bool IFIFO::Pop(UInt8 * buf, UInt32 length)
{
	bool	result = Peek(buf, length);

	// update pointers if we were successful: shrink the data run and advance (wrapping) the start
	// update pointers if we were successful
	if(result)
	{
		fifoDataLength -= length;
		fifoBase = ToRawOffset(fifoBase + length);
	}

	return result;
}

/**
 * Like Pop() but leaves the data in the queue.
 *
 * @return true if `length` bytes were copied; false if fewer are queued.
 */
bool IFIFO::Peek(UInt8 * buf, UInt32 length)
{
	// would that underflow the buffer?
	if(length > fifoDataLength)
		return false;

	// will this cross the end of the buffer? Copy the tail piece then the wrapped piece.
	// will this cross the end of the buffer?
	if(fifoBase + length > fifoBufSize)
	{
		UInt32	segmentLength = fifoBufSize - fifoBase;

		std::memcpy(buf, &fifoBuf[fifoBase], segmentLength);
		std::memcpy(&buf[segmentLength], fifoBuf, length - segmentLength);
	}
	else
	{
		std::memcpy(buf, &fifoBuf[fifoBase], length);
	}

	return true;
}

/// Discard all queued data (the buffer memory is kept).
void IFIFO::Clear(void)
{
	fifoDataLength = 0;

	// this isn't needed, but staying away from the buffer end is always good
	fifoBase = 0;
}

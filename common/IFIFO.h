// ==============================================================================
// Program: IFIFO.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: First-in first-out circular queue data structure header for thread-safe messaging.
//          Inputs: Generic element payloads and queue capacity parameters.
//          Outputs: Enqueued/dequeued elements maintaining strict insertion ordering.
//          Architecture: Work item dispatch queue between loader and game threads.
// ==============================================================================


#pragma once

/**
 * @class IFIFO
 *
 * Fixed-capacity byte ring buffer for queueing raw bytes (for example partial messages from
 * a pipe). Operations are all-or-nothing and the class is not thread-safe.
 * Virtual so a thread-safe or message-aware subclass can override the operations.
 */
// ---------------------------------------------------------------------------
// IFIFO: Ring buffer queue implementation for FIFO message buffering
// ---------------------------------------------------------------------------
class IFIFO
{
	public:
		/// @param length Capacity in bytes (must be > 0).
		IFIFO(UInt32 length = 0);
		virtual ~IFIFO();

		/// Copy `length` bytes in; @return false (nothing stored) if there is not enough free space.
		virtual bool	Push(UInt8 * buf, UInt32 length);
		/// Copy `length` bytes out and remove them; @return false (nothing removed) if fewer are queued.
		virtual bool	Pop(UInt8 * buf, UInt32 length);
		/// Copy `length` bytes out without removing them; @return false if fewer are queued.
		virtual bool	Peek(UInt8 * buf, UInt32 length);
		/// Drop all queued data.
		virtual void	Clear(void);

		/// @return Total capacity in bytes.
		UInt32	GetBufferSize(void)		{ return fifoBufSize; }
		/// @return Free space in bytes (capacity minus queued).
		UInt32	GetBufferRemain(void)	{ return fifoBufSize - fifoDataLength; }
		/// @return Bytes currently queued.
		UInt32	GetDataLength(void)		{ return fifoDataLength; }

	// Offset helpers: all positions wrap with a modulo of the capacity.
	private:
		/// Wrap any offset into [0, capacity).
		UInt32	ToRawOffset(UInt32 in)	{ return in % fifoBufSize; }
		/// Raw buffer index of the in-th queued byte (0 = oldest).
		UInt32	ToDataOffset(UInt32 in)	{ return ToRawOffset(fifoBase + in); }
		/// Raw buffer index where the next pushed byte goes (just past the newest byte).
		UInt32	GetWriteOffset(void)	{ return ToDataOffset(fifoDataLength); }

		UInt8	* fifoBuf;
		UInt32	fifoBufSize;		// size of the buffer (in bytes)
		UInt32	fifoBase;			// pointer to the beginning of the data block
		UInt32	fifoDataLength;		// size of the data block
};

// ==============================================================================
// Program: IBufferStream.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Header definition for dynamic in-memory binary byte buffer data streams.
//          Inputs: Raw memory pointers, buffer lengths, and stream read/write requests.
//          Outputs: Typed binary deserialization and auto-expanding byte buffers.
//          Architecture: SKSE64 serialization buffer for network packets and hook payload IPC.
// ==============================================================================

#pragma once

#include "common/IDataStream.h"

/**
 * @class IBufferStream
 *
 * IDataStream over caller-provided memory (despite the older banner text there is no dynamic
 * reallocation). Handy for parsing a block already in RAM, or for building a record in a fixed buffer
 * before writing it elsewhere. No bounds checking; not thread-safe.
 */
// ---------------------------------------------------------------------------
// IBufferStream: Memory-backed IDataStream implementation with dynamic reallocation
// ---------------------------------------------------------------------------
class IBufferStream : public IDataStream
{
	public:
		IBufferStream();
		/// Copy constructor (restricted, see the .cpp).
		IBufferStream(const IBufferStream & rhs);
		/// Wrap `buf` (`inLength` bytes), borrowed unless OwnBuffer() is called.
		IBufferStream(void * buf, UInt64 inLength);
		/// Releases the buffer if owned.
		virtual ~IBufferStream();

		/// Shallow assignment (restricted, see the .cpp).
		IBufferStream &	operator=(IBufferStream & rhs);

		/// Point at a different block and rewind.
		void	SetBuffer(void * buf, UInt64 inLength);
		/// @return The wrapped memory (still owned by whoever owns it).
		void *	GetBuffer(void)	{ return streamBuf; }

		/// Transfer ownership to the stream: the buffer will be delete[]'d with the stream.
		void	OwnBuffer(void)		{ flags |= kFlag_OwnedBuf; }
		/// Give ownership back to the caller.
		void	DisownBuffer(void)	{ flags &= ~kFlag_OwnedBuf; }

		// read
		virtual void	ReadBuf(void * buf, UInt32 inLength);

		// write
		virtual void	WriteBuf(const void * buf, UInt32 inLength);

	// streamBuf: the memory block; flags: ownership bits.
	protected:
		UInt8	* streamBuf;
		UInt32	flags;

		// Bit flags stored in `flags`.
		enum
		{
			kFlag_OwnedBuf = 1 << 0
		};
};

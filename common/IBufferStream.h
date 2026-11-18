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

// ---------------------------------------------------------------------------
// IBufferStream: Memory-backed IDataStream implementation with dynamic reallocation
// ---------------------------------------------------------------------------
class IBufferStream : public IDataStream
{
	public:
		IBufferStream();
		IBufferStream(const IBufferStream & rhs);
		IBufferStream(void * buf, UInt64 inLength);
		virtual ~IBufferStream();

		IBufferStream &	operator=(IBufferStream & rhs);

		void	SetBuffer(void * buf, UInt64 inLength);
		void *	GetBuffer(void)	{ return streamBuf; }

		void	OwnBuffer(void)		{ flags |= kFlag_OwnedBuf; }
		void	DisownBuffer(void)	{ flags &= ~kFlag_OwnedBuf; }

		// read
		virtual void	ReadBuf(void * buf, UInt32 inLength);

		// write
		virtual void	WriteBuf(const void * buf, UInt32 inLength);

	protected:
		UInt8	* streamBuf;
		UInt32	flags;

		enum
		{
			kFlag_OwnedBuf = 1 << 0
		};
};

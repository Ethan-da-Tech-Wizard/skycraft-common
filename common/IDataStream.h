// ==============================================================================
// Program: IDataStream.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Abstract base class specification for all binary streaming I/O interfaces.
//          Inputs: Typed data values, endianness flags, and stream buffers.
//          Outputs: Big-endian/Little-endian converted values and byte counts.
//          Architecture: Universal I/O foundation used across SKSE64 loader and game hooks.
// ==============================================================================

#pragma once

#include "common/IErrors.h"

/**
 * @class IDataStream
 *
 * Abstract binary stream: a cursor over a sequence of bytes with typed reads/writes.
 * Subclasses (IFileStream, IBufferStream, ISegmentStream, IDataSubStream, ...) implement the two
 * raw primitives ReadBuf and WriteBuf; everything else (Read8..Read64, strings, peeking,
 * skipping, copying) is built on those in IDataStream.cpp. Position and size are tracked in
 * streamOffset / streamLength.
 *
 * Byte order: values are read and written in the machine's native order unless
 * SwapBytes(true) was called, in which case multi-byte integers and floats are byte-reversed
 * (used for big-endian file formats).
 */
/**
 *	An arbitrary data stream
 */
// ---------------------------------------------------------------------------
// IDataStream: Core streaming contract defining Read, Write, Seek, and Skip
// ---------------------------------------------------------------------------
class IDataStream
{
	public:
		IDataStream();
		virtual ~IDataStream();

		// read: each call consumes the bytes and advances the offset
		// read
		virtual UInt8	Read8(void);
		virtual UInt16	Read16(void);
		virtual UInt32	Read32(void);
		virtual UInt64	Read64(void);
		virtual float	ReadFloat(void);
		/// Read a NUL-terminated (or alternate-terminator-terminated) string into buf; @return characters stored, excluding the terminator.
		virtual UInt32	ReadString(char * buf, UInt32 bufLength, char altTerminator = 0, char altTerminator2 = 0);
		/// Raw primitive: copy inLength bytes into buf and advance. Must be provided by every subclass.
		virtual void	ReadBuf(void * buf, UInt32 inLength) = 0;

		// peek: like read, but the offset is restored afterwards (via IDataStream_PositionSaver)
		// peek
		virtual UInt8	Peek8(void);
		virtual UInt16	Peek16(void);
		virtual UInt32	Peek32(void);
		virtual UInt64	Peek64(void);
		virtual float	PeekFloat(void);
		virtual void	PeekBuf(void * buf, UInt32 inLength);

		/// Advance (or, with a negative count, rewind) the offset by inBytes without transferring data.
		virtual void	Skip(SInt64 inBytes);

		// write: each call stores the bytes and advances the offset (and grows the stream when writing at the end)
		// write
		virtual void	Write8(UInt8 inData);
		virtual void	Write16(UInt16 inData);
		virtual void	Write32(UInt32 inData);
		virtual void	Write64(UInt64 inData);
		virtual void	WriteFloat(float inData);
		/// Write a NUL-terminated string including its terminating NUL byte (strlen + 1 bytes).
		virtual void	WriteString(const char * buf);
		/// Raw primitive: store inLength bytes from buf and advance. Must be provided by every subclass.
		virtual void	WriteBuf(const void * buf, UInt32 inLength) = 0;
		
		// Position / size queries, answered from the cached streamOffset and streamLength.
		SInt64			GetLength(void);
		SInt64			GetRemain(void);
		SInt64			GetOffset(void);
		/// @return true once the offset has reached the end of the stream.
		bool			HitEOF(void);

		/// Absolute seek; subclasses that wrap a file or another stream override it to move the underlying position too.
		virtual void	SetOffset(SInt64 inOffset);
		/// Seek back to offset 0.
		void			Rewind(void)	{ SetOffset(0); }

		/// Turn byte swapping of multi-byte values on or off for subsequent reads and writes.
		void			SwapBytes(bool inSwapBytes);

		// Parent chain support: a stream that is a view onto another (substream, segment stream) returns that stream here so callers can find the underlying file.
		virtual SInt64			GetParentOffset(void)	{ return GetOffset(); }
		virtual IDataStream *	GetParent(void)			{ return NULL; }

		/// @return The outermost stream in the parent chain (this stream when it has no parent).
		IDataStream *			GetRootParent(void);

		/// Rewind `in` and copy its whole length to `out`'s current position in chunks of bufferSize bytes (a temporary buffer is allocated unless the caller passes buf).
		static void		CopyStreams(IDataStream * out, IDataStream * in, UInt64 bufferSize = 1024 * 1024, UInt8 * buf = NULL);
		/// Like CopyStreams but copies exactly `remain` bytes from `in`'s CURRENT position (no rewind).
		static void		CopySubStreams(IDataStream * out, IDataStream * in, UInt64 remain, UInt64 bufferSize = 1024 * 1024, UInt8 * buf = NULL);

	// Cached stream state shared with subclasses: size, cursor, and the byte-swap flag.
	protected:
		SInt64			streamLength;
		SInt64			streamOffset;
		bool			swapBytes;
};

/**
 * A utility class to automatically save and restore the current position of an IDataStream
 * (RAII: the destructor seeks back to where the stream was when the saver was created).
 */
/**
 *	A utility class to automatically save and restore the current position of an IDataStream
 */
class IDataStream_PositionSaver
{
	public:
		IDataStream_PositionSaver(IDataStream * tgt);
		~IDataStream_PositionSaver();

	private:
		IDataStream	* stream;
		SInt64		offset;
};

/**
 * @class IDataSubStream
 *
 * A window onto a range of another stream: offset 0 of the substream is `inOffset` in the parent
 * and its length is `inLength`. All reads and writes are forwarded to the parent after
 * translating the offset by subBase. The parent is borrowed, not owned.
 */
class IDataSubStream : public IDataStream
{
	public:
		IDataSubStream();
		IDataSubStream(IDataStream * inStream, SInt64 inOffset, SInt64 inLength);
		~IDataSubStream();

		/// (Re)bind to `inStream`, covering inLength bytes starting at inOffset in the parent.
		void	Attach(IDataStream * inStream, SInt64 inOffset, SInt64 inLength);

		/// Forwarded to the parent at subBase + current offset (ReadBuf ASSERTs when reading past the window; WriteBuf grows the window length).
		void	ReadBuf(void * buf, UInt32 inLength);
		void	WriteBuf(const void * buf, UInt32 inLength);
		void	SetOffset(SInt64 inOffset);

		// Parent chain: offsets reported here are the parent's position.
		virtual SInt64			GetParentOffset(void)	{ return stream->GetOffset(); }
		virtual IDataStream *	GetParent(void)			{ return stream; }

		/// @return Where this window starts inside the parent stream.
		SInt64	GetSubBase(void)	{ return subBase; }

	private:
		IDataStream	* stream;

		SInt64	subBase;
};

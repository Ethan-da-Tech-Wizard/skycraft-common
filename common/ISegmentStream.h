// ==============================================================================
// Program: ISegmentStream.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Sub-stream windowing decorator restricting I/O to a slice of a backing stream.
//          Inputs: Backing IDataStream pointer, segment base offset, and segment size.
//          Outputs: Bounded stream I/O preventing reads/writes past segment boundaries.
//          Architecture: Asset decompression and chunked archive partition streaming.
// ==============================================================================

#pragma once

#include "common/IDataStream.h"
/*
 * Read-only stream built from several slices ("segments") of another stream; see ISegmentStream.cpp
 * for the offset-translation algorithm. Typical use: reading one logical record that is stored in
 * non-adjacent pieces of a larger file.
 */
#include <vector>

/**
 *	An stream composed of many non-contiguous segments of a larger stream
 */
// ---------------------------------------------------------------------------
// ISegmentStream: Windowed stream wrapper enforcing virtual offset boundaries
// ---------------------------------------------------------------------------
/// Create an empty segment view; call AttachStream() then AddSegment() for each piece.
class ISegmentStream : public IDataStream
{
	public:
		/// Bind to the parent stream (borrowed, not owned) and reset position/length.
		ISegmentStream();
		~ISegmentStream();
/// Map [offset, offset+length) of this stream onto [parentOffset, parentOffset+length) of the parent.

		void	AttachStream(IDataStream * inStream);
		/// Read across segment boundaries as needed (asserts if the range is not fully mapped).
		
		/// Not supported: halts.
		void	AddSegment(UInt64 offset, UInt64 length, UInt64 parentOffset);
/// Seek to a virtual offset that must lie inside a segment.

		virtual void	ReadBuf(void * buf, UInt32 inLength);
		virtual void	WriteBuf(const void * buf, UInt32 inLength);
		// The underlying stream every read is forwarded to.
		virtual void	SetOffset(SInt64 inOffset);

	/// One mapped slice. `offset` is in this stream's virtual space, `parentOffset` in the parent's.
	protected:
		IDataStream	* parent;

		struct SegmentInfo
		{
			SegmentInfo(UInt64 inOffset, UInt64 inLength, UInt64 inParentOffset)
			{
				offset = inOffset;
				length = inLength;
				parentOffset = inParentOffset;
			}

			UInt64	offset;
			UInt64	length;
			UInt64	parentOffset;
		};

		// Segments in insertion order; LookupInfo scans them linearly.
		typedef std::vector <SegmentInfo>	SegmentInfoListType;
		SegmentInfoListType					segmentInfo;

		/// @return The segment containing the virtual offset, or NULL.
		SegmentInfo *	LookupInfo(UInt64 offset);
};

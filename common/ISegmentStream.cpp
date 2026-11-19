// ==============================================================================
// Program: ISegmentStream.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of bounded window stream reading, writing, and cursor mapping.
//          Inputs: Relative segment offsets.
//          Outputs: Clamped read/write byte counts forwarded to backing stream.
//          Algorithm: Relative offset translation with boundary clamping.
// ==============================================================================


#include "common/ISegmentStream.h"

/*
 * Model: the stream presents one contiguous virtual address space (0..streamLength) that is
 * stitched together from segments, each mapping [offset, offset+length) of this stream onto
 * [parentOffset, parentOffset+length) of an underlying "parent" stream. Reads are split at
 * segment boundaries and forwarded to the parent. Read-only.
 */

// ---------------------------------------------------------------------------
// Constructor: Binds to parent stream and defines valid window range
// ---------------------------------------------------------------------------
/// Start empty. NOTE: `parent` and `streamOffset` are not initialised here; call AttachStream() before any use.
ISegmentStream::ISegmentStream()
{
	streamLength = 0;
}

/// Nothing to release: the parent stream is borrowed, not owned, and segments are held in a vector.
ISegmentStream::~ISegmentStream()
{

}

/**
 * Bind to the stream that holds the real data and forget all previous layout bookkeeping
 * (offset and length reset; previously added segments are NOT cleared).
 *
 * @param inStream Parent stream; must outlive this object (not owned).
 */
void ISegmentStream::AttachStream(IDataStream * inStream)
{
	parent = inStream;
	streamLength = 0;
	streamOffset = 0;
}

/**
 * Map a slice of the parent into this stream's virtual space.
 *
 * @param offset       Where the segment starts in this stream's virtual space.
 * @param length       Segment size in bytes.
 * @param parentOffset Where the same bytes start in the parent stream.
 *
 * Segments must not overlap in virtual space (LookupInfo returns the first match).
 * streamLength is grown to cover the segment; note it is derived from parentOffset + length,
 * which equals the virtual end only when the virtual and parent offsets agree.
 */
void ISegmentStream::AddSegment(UInt64 offset, UInt64 length, UInt64 parentOffset)
{
	segmentInfo.push_back(SegmentInfo(offset, length, parentOffset));
	
	if(streamLength < (parentOffset + length))
		streamLength = parentOffset + length;
}

/**
 * Read inLength bytes from the current virtual position, crossing segment boundaries as needed.
 *
 * @param buf      Destination, at least inLength bytes.
 * @param inLength Bytes to read. ASSERTs (aborts) if the range runs into a gap or past the last segment.
 *
 * Side effect: moves the parent's file pointer and advances streamOffset.
 */
void ISegmentStream::ReadBuf(void * buf, UInt32 inLength)
{
	UInt32	remain = inLength;
	UInt8	* out = (UInt8 *)buf;

	// One iteration per segment touched; each pass copies at most the rest of the current segment.
	while(remain > 0)
	{
		SegmentInfo	* info = LookupInfo(streamOffset);
		// ASSERT: reading outside every segment is a caller bug (no sensible data to return).
		ASSERT(info);

		// Position inside the current segment, and how many bytes remain before its end.
		UInt64		segmentOffset = streamOffset - info->offset;
		UInt64		transferLength = info->length - segmentOffset;

		// Do not read past the caller's request.
		if(transferLength > remain)
			transferLength = remain;

		// Seek the parent every time: other users of the parent may have moved its pointer between our reads.
		parent->SetOffset(info->parentOffset + segmentOffset);
		parent->ReadBuf(out, transferLength);

		// NOTE: `out` is never advanced, so a read that spans two segments overwrites the start of the buffer
		// instead of appending. Left unchanged in this comments-only pass.
		streamOffset += transferLength;
		remain -= transferLength;
	}
}

/// Writing is not supported on a segment view; halts with an error message.
void ISegmentStream::WriteBuf(const void * buf, UInt32 inLength)
{
	HALT("ISegmentStream::WriteBuf: writing unsupported");
}

/**
 * Move the virtual read position.
 *
 * @param inOffset Absolute virtual offset; must lie inside a segment (ASSERT otherwise).
 */
void ISegmentStream::SetOffset(SInt64 inOffset)
{
	SegmentInfo	* info = LookupInfo(inOffset);
	ASSERT(info);

	UInt64		segmentOffset = inOffset - info->offset;

	parent->SetOffset(info->parentOffset + segmentOffset);

	streamOffset = inOffset;
}

/**
 * Find the segment containing a virtual offset.
 *
 * @param offset Virtual offset.
 * @return Pointer into segmentInfo (invalidated by AddSegment), or NULL if no segment covers it.
 * Linear scan: fine for the handful of segments this class is used with.
 */
ISegmentStream::SegmentInfo * ISegmentStream::LookupInfo(UInt64 offset)
{
	for(SegmentInfoListType::iterator iter = segmentInfo.begin(); iter != segmentInfo.end(); iter++)
		if((offset >= (*iter).offset) && (offset < (*iter).offset + (*iter).length))
			return &(*iter);

	return NULL;
}

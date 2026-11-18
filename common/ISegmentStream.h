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
#include <vector>

/**
 *	An stream composed of many non-contiguous segments of a larger stream
 */
// ---------------------------------------------------------------------------
// ISegmentStream: Windowed stream wrapper enforcing virtual offset boundaries
// ---------------------------------------------------------------------------
class ISegmentStream : public IDataStream
{
	public:
		ISegmentStream();
		~ISegmentStream();

		void	AttachStream(IDataStream * inStream);
		
		void	AddSegment(UInt64 offset, UInt64 length, UInt64 parentOffset);

		virtual void	ReadBuf(void * buf, UInt32 inLength);
		virtual void	WriteBuf(const void * buf, UInt32 inLength);
		virtual void	SetOffset(SInt64 inOffset);

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

		typedef std::vector <SegmentInfo>	SegmentInfoListType;
		SegmentInfoListType					segmentInfo;

		SegmentInfo *	LookupInfo(UInt64 offset);
};

// ==============================================================================
// Program: IArchive.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of Bethesda Archive (BSA) asset decompression and file extraction.
//          Inputs: Raw archive file handles and directory offset tables.
//          Outputs: In-memory asset buffers and decompression status codes.
//          Algorithm: Binary record traversal with zlib/LZ4 stream decompression.
// ==============================================================================

#include "IArchive.h"
#include "IErrors.h"

#if ENABLE_IDYNAMICCREATE

IDynamic * IArchive::iterator::Instantiate(void)
{
	IDataSubStream	subStream(owner->theStream, GetDataOffset(), GetDataLength());

	return IClassRegistry::Instantiate(GetTypeID(), &subStream);
}

void * IArchive::iterator::GetBuffer(UInt32 * outLength)
{
	HeaderEntry	* entry = GetData();
	UInt8		* buf = new UInt8[entry->dataLength];

	owner->theStream->SetOffset(entry->dataOffset);
	owner->theStream->ReadBuf(buf, entry->dataLength);

	if(outLength)
		*outLength = entry->dataLength;

	return buf;
}

void IArchive::iterator::NextOfType(UInt32 typeID)
{
	idx++;

	while((GetData()->typeID != typeID) && (idx < owner->header.numEntries))
		idx++;
}

void IArchive::iterator::PrevOfType(UInt32 typeID)
{
	idx--;

	while((GetData()->typeID != typeID) && (idx > 0))
		idx--;
}

// ---------------------------------------------------------------------------
// Constructor: Initializes archive state, file handles, and buffer pointers
// ---------------------------------------------------------------------------
IArchive::IArchive()
:theStream(NULL), entries(NULL), nameTable(NULL)
{

}

IArchive::IArchive(IDataStream * stream)
:theStream(NULL), entries(NULL), nameTable(NULL)
{
	AttachStream(stream);
}

IArchive::~IArchive()
{
	Dispose();
}

void IArchive::AttachStream(IDataStream * inStream)
{
	Dispose();

	theStream = inStream;
}

void IArchive::Dispose(void)
{
	if(entries)
	{
		delete entries;
		entries = NULL;
	}

	if(nameTable)
	{
		delete nameTable;
		nameTable = NULL;
	}
}

void IArchive::ReadHeader(void)
{
	ASSERT(theStream);

	theStream->Rewind();

	theStream->ReadBuf(&header, sizeof(FileHeader));

	entries = new HeaderEntry[header.numEntries];
	theStream->ReadBuf(entries, header.numEntries * sizeof(HeaderEntry));

	if(header.nameTableLength)
	{
		nameTable = new char[header.nameTableLength];

		theStream->SetOffset(header.nameTableOffset);
		theStream->ReadBuf(nameTable, header.nameTableLength);
	}
}

#endif

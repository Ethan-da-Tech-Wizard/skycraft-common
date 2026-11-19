// ==============================================================================
// Program: IArchive.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of Bethesda Archive (BSA) asset decompression and file extraction.
//          Inputs: Raw archive file handles and directory offset tables.
//          Outputs: In-memory asset buffers and decompression status codes.
//          Algorithm: Binary record traversal with zlib/LZ4 stream decompression.
// ==============================================================================

// NOTE on the header above: this file only reads the archive's header, entry
// table and (optional) name table into memory and gives access to each entry's
// raw bytes; it performs no zlib/LZ4 decompression itself. The whole
// implementation is compiled only when ENABLE_IDYNAMICCREATE is set.
#include "IArchive.h"
#include "IErrors.h"

#if ENABLE_IDYNAMICCREATE

// Create an object from the current entry: the entry's byte range is exposed
// as a sub-stream and handed to the class registry, which builds the object
// registered for the entry's type id.
IDynamic * IArchive::iterator::Instantiate(void)
{
	IDataSubStream	subStream(owner->theStream, GetDataOffset(), GetDataLength());

	return IClassRegistry::Instantiate(GetTypeID(), &subStream);
}

// Read the whole entry into a new[] buffer (the caller must delete[] it) and
// optionally report its length.
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

// Advance to the next entry with the given type id. NOTE: the type test reads
// the entry at the new index before the bounds check, so the caller should not
// step past the last entry.
void IArchive::iterator::NextOfType(UInt32 typeID)
{
	idx++;

	while((GetData()->typeID != typeID) && (idx < owner->header.numEntries))
		idx++;
}

// Mirror of NextOfType, stopping at index 0.
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

// Adopt a stream (not owned) after releasing any previously loaded tables.
void IArchive::AttachStream(IDataStream * inStream)
{
	Dispose();

	theStream = inStream;
}

// Frees the entry and name tables (the stream is left alone).
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

// Layout read here: FileHeader, then numEntries HeaderEntry records, and the
// name table from its recorded offset when one exists.
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

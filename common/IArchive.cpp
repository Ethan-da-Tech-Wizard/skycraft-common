// ==============================================================================
// Program: IArchive.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Reading and instantiating objects from an "object archive": a
//          simple SKSE container with a 5-field header, a table of entries and an
//          optional name table (see IArchive.h for the exact layout).
//          Inputs: an IDataStream positioned over the archive.
//          Outputs: per-entry metadata, raw entry bytes (GetBuffer) or objects
//          built through the IClassRegistry (Instantiate).
//          NOT done here: this is not the Bethesda BSA format and nothing in
//          this file performs zlib/LZ4 (or any) decompression; entries are
//          returned exactly as stored.
//
// How it fits: IArchive.h declares the class; IDynamicCreate.h supplies
// IClassRegistry and IDynamic, which Instantiate() uses to turn an entry into
// an object. The whole implementation is compiled only when
// ENABLE_IDYNAMICCREATE is non-zero. IDynamicCreate.h currently defines it as 0
// ("this screws with edit-and-continue and we don't use it"), so in the
// current build this file compiles to nothing and IArchive is unused.
//
// Known limits: no bounds checks on header fields (a corrupt file can make
// ReadHeader allocate a huge table); NextOfType/PrevOfType read an entry
// before testing the index; names are not validated for a terminating NUL.
// ==============================================================================

#include "IArchive.h"
#include "IErrors.h"

#if ENABLE_IDYNAMICCREATE

/**
 * Creates an object from the current entry.
 *
 * The entry's byte range is exposed as a sub-stream and handed to the class
 * registry, which builds the object registered for the entry's type id.
 * @return New object owned by the caller (see IClassRegistry::Instantiate in
 *         IDynamicCreate.cpp for the behaviour on an unknown type id).
 */
IDynamic * IArchive::iterator::Instantiate(void)
{
	IDataSubStream	subStream(owner->theStream, GetDataOffset(), GetDataLength());

	return IClassRegistry::Instantiate(GetTypeID(), &subStream);
}

/**
 * Reads the whole current entry into a new[] buffer.
 * @param outLength [out, optional] Receives the entry length in bytes.
 * @return Buffer of dataLength bytes. The caller owns it and must release it
 *         with delete[]. The stream offset is left just after the entry.
 */
void * IArchive::iterator::GetBuffer(UInt32 * outLength)
{
	HeaderEntry	* entry = GetData();
	UInt8		* buf = new UInt8[entry->dataLength];

	// Seek (absolute) to the entry's data, then read it in one call.
	owner->theStream->SetOffset(entry->dataOffset);
	owner->theStream->ReadBuf(buf, entry->dataLength);

	if(outLength)
		*outLength = entry->dataLength;

	return buf;
}

/**
 * Advances to the next entry whose type id equals @p typeID.
 * Stops at index numEntries (end()) when there is none.
 * NOTE: the type test reads the entry at the new index before the bounds
 * check, so the caller should not step past the last entry.
 */
void IArchive::iterator::NextOfType(UInt32 typeID)
{
	idx++;

	while((GetData()->typeID != typeID) && (idx < owner->header.numEntries))
		idx++;
}

/** Mirror of NextOfType() going backwards; stops at index 0 (which is not tested for a match). */
void IArchive::iterator::PrevOfType(UInt32 typeID)
{
	idx--;

	while((GetData()->typeID != typeID) && (idx > 0))
		idx--;
}

/** Constructs an empty archive; call AttachStream() before use. */
IArchive::IArchive()
:theStream(NULL), entries(NULL), nameTable(NULL)
{

}

/** Constructs an archive over @p stream (not owned; it must outlive this object). */
IArchive::IArchive(IDataStream * stream)
:theStream(NULL), entries(NULL), nameTable(NULL)
{
	AttachStream(stream);
}

/** Releases the entry and name tables; the stream is not touched. */
IArchive::~IArchive()
{
	Dispose();
}

/**
 * Adopts a stream (not owned) after releasing any previously loaded tables.
 * Does not read the header; call ReadHeader() next.
 */
void IArchive::AttachStream(IDataStream * inStream)
{
	Dispose();

	theStream = inStream;
}

/** Frees the entry and name tables (the stream is left alone). Safe to call repeatedly. */
void IArchive::Dispose(void)
{
	if(entries)
	{
		delete [] entries;
		entries = NULL;
	}

	if(nameTable)
	{
		delete [] nameTable;
		nameTable = NULL;
	}
}

/**
 * Loads the header, entry table and (if present) name table from the stream.
 *
 * Layout read: FileHeader (5 x UInt32), then numEntries HeaderEntry records
 * (5 x UInt32 each), and the name table from header.nameTableOffset when
 * header.nameTableLength is non-zero. Asserts if no stream is attached.
 * Does not verify fileID or version.
 */
void IArchive::ReadHeader(void)
{
	ASSERT(theStream);

	// Header is at the start of the stream.
	theStream->Rewind();

	theStream->ReadBuf(&header, sizeof(FileHeader));

	// One HeaderEntry per entry, read back-to-back right after the header.
	entries = new HeaderEntry[header.numEntries];
	theStream->ReadBuf(entries, header.numEntries * sizeof(HeaderEntry));

	// The name table is optional; entries index into it by byte offset (nameOffset).
	if(header.nameTableLength)
	{
		nameTable = new char[header.nameTableLength];

		theStream->SetOffset(header.nameTableOffset);
		theStream->ReadBuf(nameTable, header.nameTableLength);
	}
}

#endif

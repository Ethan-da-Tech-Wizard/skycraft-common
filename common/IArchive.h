// ==============================================================================
// Program: IArchive.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Declaration of IArchive, a reader for SKSE's own "object archive"
//          container (not the Bethesda BSA format).
//          Inputs: an IDataStream over the archive file.
//          Outputs: an iterator over entries giving type id, sub id, name, raw
//          bytes (GetBuffer) or an instantiated IDynamic object (Instantiate).
//          Architecture: header + entry table + optional name table; entries
//          are stored uncompressed and this class never decompresses anything.
//
// File layout (all fields UInt32, in stream order):
//   FileHeader   fileID (kFileID), version (kCurrentVersion), numEntries,
//                nameTableOffset, nameTableLength
//   HeaderEntry  x numEntries: typeID, subID, dataOffset, dataLength, nameOffset
//   entry data at each dataOffset; name table (NUL-terminated strings) at
//   nameTableOffset, indexed by each entry's nameOffset.
// Compiled only when ENABLE_IDYNAMICCREATE is non-zero (currently 0, see
// IDynamicCreate.h), so nothing here is active in the default build.
// ==============================================================================

#pragma once

#include "common/IDataStream.h"
#include "common/IDynamicCreate.h"

#if ENABLE_IDYNAMICCREATE

/**
 *	An object archive
 *	
 *	This class implements reading and instantiating objects from an object archive.
 *	It does not own the stream. Intended use: AttachStream(), then ReadHeader(),
 *	then walk begin()..end(). NOTE: ReadHeader() is private and nothing in this
 *	class calls it, so as written the tables are never loaded; the class is
 *	dead code while ENABLE_IDYNAMICCREATE is 0.
 */
/**
 *	An object archive
 *	
 *	This class implements reading and instantiating objects from an object archive.
 */
// Note: the destructor is not virtual; do not delete through a base pointer.
class IArchive
{
	public:
		class iterator;
		friend	iterator;

		IArchive();
		IArchive(IDataStream * inStream);
		~IArchive();

		// Takes a non-owning stream pointer; Dispose() is called first so a re-attach
		// never leaks tables loaded from the previous stream.
		void	AttachStream(IDataStream * inStream);
		void	Dispose(void);

		// begin()/end() give a half-open range [0, numEntries) usable with ++/--.
		iterator	begin(void)	{ return iterator(0, this); }
		iterator	end(void)	{ return iterator(header.numEntries, this); }

		// Magic number: bytes 00 'A' 'R' 01 packed with CHAR_CODE (first byte lowest).
		static const UInt32	kFileID = CHAR_CODE(0x00, 'A', 'R', 0x01);
		// Format version 1.0.0 packed as 12/12/8 bits (see VERSION_CODE in ITypes.h).
		static const UInt32	kCurrentVersion = VERSION_CODE(1, 0, 0);

	// On-disk structures. They are read straight into memory with ReadBuf, so
	// their layout (five 32-bit fields each, no padding) must not change.
	private:
		struct FileHeader
		{
			UInt32	fileID;		// IArchive::kFileID
			UInt32	version;	// IArchive::kCurrentVersion
			UInt32	numEntries;
			UInt32	nameTableOffset;
			UInt32	nameTableLength;
		};

		// One table record: what the entry is (typeID selects the registered class,
		// subID is a type-specific discriminator), where its bytes are (offset from
		// the start of the stream, length) and where its name starts in nameTable.
		// BSA archive header layout describing version, folder count, and flag bitmasks
		struct HeaderEntry
		{
			UInt32	typeID;
			UInt32	subID;
			UInt32	dataOffset;
			UInt32	dataLength;
			UInt32	nameOffset;
		};

		void	ReadHeader(void);

		// Not owned; supplied by the caller and must outlive the archive.
		IDataStream	* theStream;

		// Header and heap tables; entries/nameTable are NULL until ReadHeader() runs.
		FileHeader	header;
		HeaderEntry	* entries;

		char		* nameTable;

	public:
		/**
		 * Cursor over the entries. Holds an index and its owning archive; copyable.
		 * No bounds checking: callers compare with end() themselves.
		 */
		class iterator
		{
			public:
				iterator()										{ idx = 0; owner = NULL; }
				iterator(UInt32 inIdx, IArchive * inArchive)	{ idx = inIdx; owner = inArchive; }
				~iterator()										{ }

				// Builds an object from the entry's bytes via IClassRegistry.
				IDynamic *	Instantiate(void);

				UInt32		GetTypeID(void)			{ return GetData()->typeID; }
				UInt32		GetSubID(void)			{ return GetData()->subID; }
				UInt32		GetDataLength(void)		{ return GetData()->dataLength; }
				// NUL-terminated name inside the archive's name table (only valid when the
				// archive has a name table).
				char *		GetName(void)			{ return &owner->nameTable[GetData()->nameOffset]; }
				// Returns a new[] copy of the entry's bytes; caller delete[]s it.
				void *		GetBuffer(UInt32 * outLength);

				iterator &	operator++() { Next(); return *this; }
				iterator &	operator--() { Prev(); return *this; }

				void		NextOfType(UInt32 typeID);
				void		Next(void)	{ idx++; }

				void		PrevOfType(UInt32 typeID);
				void		Prev(void)	{ idx--; }

			private:
				HeaderEntry *	GetData(void)	{ return &owner->entries[idx]; }

				UInt32		GetDataOffset(void)	{ return GetData()->dataOffset; }

				UInt32		idx;
				IArchive	* owner;
		};
};

#endif

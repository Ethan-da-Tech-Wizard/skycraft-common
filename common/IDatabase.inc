/**
 * @file IDatabase.inc
 *
 * Out-of-line member definitions of the IDatabase<DataType> class template (declared in
 * IDatabase.h, which #includes this file at its end). They live in a .inc because template
 * members must be visible to every translation unit that instantiates them.
 *
 * On-disk/stream format written by Save and read by Load (native byte order):
 *   UInt32  entry count
 *   UInt64  newKeyHint (next key the database will try to hand out)
 *   then, per entry: UInt64 key, followed by sizeof(DataType) raw bytes of the value.
 * Values are copied as raw bytes, so DataType must be plain data (no pointers to owned
 * memory, no virtual functions, no std::string members); the file is not portable
 * between builds with different struct layout or endianness.
 */

/**
 * Serialize the whole map to a data stream.
 *
 * @param stream Destination, positioned where the database should start. Not rewound or closed.
 */
template <class DataType>
void IDatabase <DataType>::Save(IDataStream * stream)
{
	// Header first so Load knows how many records to expect before reading any of them.
	stream->Write32(theDataMap.size());
	stream->Write64(newKeyHint);
	
	// std::map iterates in key order, so the file contents are deterministic for a given database.
	for(DataMapType::iterator iter = theDataMap.begin(); iter != theDataMap.end(); iter++)
	{
		stream->Write64((*iter).first);
		stream->WriteBuf(&((*iter).second), sizeof(DataType));
	}
}

/**
 * Replace the current contents with a database read from a stream (inverse of Save).
 *
 * @param stream Source positioned at the start of a Save() image. Truncated or corrupt data is
 *               not detected here; the stream's own error handling applies.
 */
template <class DataType>
void IDatabase <DataType>::Load(IDataStream * stream)
{
	UInt32	numEntries = stream->Read32();
	newKeyHint = stream->Read64();
	
	// Discard old entries first so stale keys cannot survive a load.
	theDataMap.clear();
	
	for(UInt32 i = 0; i < numEntries; i++)
	{
		// operator[] default-constructs the slot, then ReadBuf overwrites its raw bytes in place.
		UInt64	key = stream->Read64();
		stream->ReadBuf(&(theDataMap[key]), sizeof(DataType));
	}
}

/**
 * Save the database to a file, replacing any existing file.
 *
 * @param name Path of the file to create.
 * @return true on success; false if the file could not be created.
 */
template <class DataType>
bool IDatabase <DataType>::SaveToFile(char * name)
{
	IFileStream	stream;

	if(stream.Create(name))
	{
		Save(&stream);
		return true;
	}

	return false;
}

/**
 * Load the database from an existing file.
 *
 * @param name Path of a file written by SaveToFile.
 * @return true if the file opened and was read; false if it could not be opened
 *         (the current contents are then left unchanged).
 */
template <class DataType>
bool IDatabase <DataType>::LoadFromFile(char * name)
{
	IFileStream	stream;

	if(stream.Open(name))
	{
		Load(&stream);
		return true;
	}

	return false;
}

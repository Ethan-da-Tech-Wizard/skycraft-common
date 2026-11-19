// ==============================================================================
// Program: IDatabase.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Header definition for persistent key-value and record-indexed data storage.
//          Inputs: Record IDs, table definitions, and serialized field values.
//          Outputs: Fast in-memory indexing and structured binary table files.
//          Architecture: Local caching and state persistence for mod frameworks.
// ==============================================================================

#pragma once

#include <map>
#include "common/IDataStream.h"
#include "common/IFilestream.h"

// IDatabase<DataType>: an in-memory table of records keyed by 60-bit ids,
// stored in a std::map, with Save/Load to an IDataStream or a file. Key 0 is
// never valid. The top four bits of a key are masked off (kGUIDMask), so a key
// may carry flags there that the database ignores.
template <class DataType>
// ---------------------------------------------------------------------------
// IDatabase: Index-based record table storage and binary serialization manager
// ---------------------------------------------------------------------------
class IDatabase
{
	public:
		typedef std::map <UInt64, DataType>		DataMapType;
		typedef typename DataMapType::iterator	DataMapIterator;

		static const UInt64	kGUIDMask = 0x0FFFFFFFFFFFFFFF;

		IDatabase()	{ newKeyHint = 1; }
		virtual ~IDatabase()	{ }

		// Lookup; returns NULL for key 0 or an unknown key.
		DataType *	Get(UInt64 key)
		{
			key &= kGUIDMask;

			if(!key)
				return NULL;

			DataMapType::iterator	iter = theDataMap.find(key);

			return (iter == theDataMap.end()) ? NULL : &((*iter).second);
		}

		// Create a record at a caller-chosen key; returns NULL if the key is 0 or already used.
		DataType *	Alloc(UInt64 key)
		{
			key &= kGUIDMask;

			if(!key)
				return NULL;

			DataMapType::iterator	iter = theDataMap.find(key);

			return (iter == theDataMap.end()) ? &theDataMap[key] : NULL;
		}

		// Create a record at the next free key and return it through *key. Starts at
		// newKeyHint and, on a collision, looks for the first gap after the occupied
		// key (wrapping to 1 at the end of the map), so freed ids get reused.
		DataType *	Alloc(UInt64 * key)
		{
			UInt64	newKey = newKeyHint;

			do
			{
				if(!newKey)
					newKey++;

				DataMapType::iterator	iter = theDataMap.find(newKey);

				// is 'newKey' unused?
				if(iter == theDataMap.end())
				{
					*key = newKey;
					newKeyHint = (newKey + 1) & kGUIDMask;
					return &theDataMap[newKey];
				}
				else
				{
					++iter;
					if(iter == theDataMap.end())
					{
						newKey = 1;
					}
					else
					{
						UInt64	nextKey = (newKey + 1) & kGUIDMask;
						if(iter->first != nextKey)
						{
							*key = nextKey;
							newKeyHint = (nextKey + 1) & kGUIDMask;
							return &theDataMap[nextKey];
						}
					}
				}
			}
			while(1);

			*key = 0;

			return NULL;
		}

		// Erase a record and remember its key as the next allocation hint.
		void		Delete(UInt64 key)
		{
			if(key)
			{
				key &= kGUIDMask;

				theDataMap.erase(key);

				newKeyHint = key;
			}
		}

		// Serialisation of the whole table (implemented after the class).
		void		Save(IDataStream * stream);
		void		Load(IDataStream * stream);

		bool		SaveToFile(char * name);
		bool		LoadFromFile(char * name);

		DataMapType &	GetData(void)		{ return theDataMap; }

		DataMapIterator	Begin(void)		{ return theDataMap.begin(); }
		DataMapIterator	End(void)		{ return theDataMap.end(); }
		UInt32			Length(void)	{ return theDataMap.size(); }

	private:
		DataMapType	theDataMap;
		UInt64		newKeyHint;
};

#include "common/IDatabase.inc"

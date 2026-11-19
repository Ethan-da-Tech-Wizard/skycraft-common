// ==============================================================================
// Program: IRangeMap.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Memory address range mapping and interval tree container header.
//          Inputs: Virtual memory start addresses, lengths, and associated payloads.
//          Outputs: Fast interval lookup identifying memory regions for given pointers.
//          Architecture: Manages hook branch trampolines and patch address reservations.
// ==============================================================================


#pragma once

#include <map>

/**
 * @class IRangeMap
 *
 * Map of NON-overlapping ranges [start, start+length-1] to a user payload, with point lookup
 * ("which range contains address X?") in O(log n). Built on std::map keyed by range start.
 * Typical use: tracking which block of memory or which id span an address belongs to.
 *
 * @tparam t_key  Numeric type for positions (addresses/ids).
 * @tparam t_data Payload stored per range; default-constructed on Add.
 *
 * Limits: a range cannot span the whole key space (the end is computed as start+length-1), and
 * ranges may not overlap (Add refuses). Not thread-safe.
 * ### you can't create a range taking up the entire range of t_key
 * ### (could be done by switching from start/length -> start/end)
 */
template <typename t_key, typename t_data>
// ---------------------------------------------------------------------------
// IRangeMap: Disjoint interval map indexing memory address spans
// ---------------------------------------------------------------------------
class IRangeMap
{
public:
	/// One stored range: its length and the payload. The range start is the map key.
	struct Entry
	{
		/// @return true if addr lies within [base, base + length - 1]; base is the entry's map key, passed in because Entry does not store it.
		bool	Contains(t_key addr, t_key base)
		{
			return (addr >= base) && (addr <= (base + length - 1));
		}

		t_key	length;
		t_data	data;
	};

	// Iterator over the underlying map; `first` is a range start, `second` is the Entry.
	typedef std::map <t_key, Entry>			EntryMapType;
	typedef typename EntryMapType::iterator	Iterator;

	/// Empty map.
	IRangeMap()
	{
		//
	}

	/// Virtual so subclasses can add behavior.
	virtual ~IRangeMap()
	{
		//
	}

	/// Remove every range.
	void	Clear(void)
	{
		m_entries.clear();
	}

	/**
	 * Insert a new range.
	 *
	 * @param start  First position of the range.
	 * @param length Number of positions (must be >= 1).
	 * @return Pointer to the new range's payload (stable until that range is erased), or NULL if the
	 *         range would overlap an existing one or overflow t_key.
	 *
	 * Strategy: find the first existing range at or after `start`; the new range fits only if that
	 * range starts after the new end and the range before it ends before `start`.
	 */
	t_data *	Add(t_key start, t_key length)
	{
		t_data	* result = NULL;
		Entry	* entry = NULL;

		// Last position of the new range; if start+length-1 wrapped around it is below start, which is rejected.
		t_key	end = start + length - 1;

		if(end >= start)	// check for overflow ### should also check for overflow on length - 1, but that's pedantic
		{
			// special-case empty lists: nothing can collide
			// special-case empty lists
			if(m_entries.empty())
			{
				entry = &m_entries[start];
			}
			else
			{
				// collision check (only the two neighbours of the insertion point can overlap, because existing ranges are disjoint)
				// collision check

				EntryMapType::iterator	iter = m_entries.lower_bound(start);
				// iter contains the first entry at or after start (or null)

				if(iter == m_entries.begin())
				{
					// there can't be anything before this entry
					// so we only need to check if it's colliding with us

					if(iter->first > end)
					{
						// can't provide a hint because we're inserting at the top
						entry = &m_entries[start];
					}
				}
				else
				{
					// see if this entry doesn't collide
					// can be null (null entries don't collide)
					if((iter == m_entries.end()) || (iter->first > end))
					{
						// we didn't get the first entry in the map
						// and there is at least one entry in the map
						// therefore there's an entry before iter
						EntryMapType::iterator	preIter = iter;
						preIter--;

						// check if this collides
						// guaranteed to be the first entry before start
						t_key	preEnd = preIter->first + preIter->second.length - 1;

						if(preEnd < start)
						{
							// cool, everything's fine, allocate it
							EntryMapType::iterator	newEntry = m_entries.insert(preIter, EntryMapType::value_type(start, Entry()));
							entry = &newEntry->second;
						}
					}
				}
			}
		}

		// set up the entry: the map slot already exists (default-constructed); record its length
		// set up the entry
		if(entry)
		{
			entry->length = length;

			result = &entry->data;
		}

		return result;
	}

	/**
	 * Find the range containing an address.
	 *
	 * @param addr   Position to look up.
	 * @param base   Optional out: start of the containing range.
	 * @param length Optional out: length of the containing range.
	 * @return Pointer to the range's payload, or NULL if addr is in no range.
	 */
	t_data *	Lookup(t_key addr, t_key * base = NULL, t_key * length = NULL)
	{
		t_data	* result = NULL;

		EntryMapType::iterator	iter = LookupIter(addr);
		if(iter != m_entries.end())
		{
			if(base) *base = iter->first;
			if(length) *length = iter->second.length;

			result = &iter->second.data;
		}

		return result;
	}

	/**
	 * Remove the range containing an address.
	 *
	 * @param addr   Any position inside the range.
	 * @param base   Optional out: start of the removed range.
	 * @param length Optional out: length of the removed range.
	 * @return true if a range was removed; false if addr was in no range.
	 */
	bool	Erase(t_key addr, t_key * base = NULL, t_key * length = NULL)
	{
		bool result = false;

		EntryMapType::iterator	iter = LookupIter(addr);
		if(iter != m_entries.end())
		{
			if(base) *base = iter->first;
			if(length) *length = iter->second.length;

			m_entries.erase(iter);

			result = true;
		}

		return result;
	}

	/**
	 * Length of the range a payload belongs to, given only the payload pointer returned by Add/Lookup.
	 * Works by stepping back from the payload to its enclosing Entry with offsetof, so `data` must really
	 * point at an Entry::data member of this map.
	 */
	t_key	GetDataRangeLength(t_data * data)
	{
		Entry	* entry = reinterpret_cast <Entry *>(reinterpret_cast <UInt8 *>(data) - offsetof(Entry, data));

		return entry->length;
	}

	/**
	 * Like Lookup, but returns the map iterator (to get the range start via ->first).
	 *
	 * @return Iterator to the containing entry, or End() if none.
	 */
	typename EntryMapType::iterator	LookupIter(t_key addr)
	{
		EntryMapType::iterator	result = m_entries.end();

		if(!m_entries.empty())
		{
			// we need to find the last entry less than or equal to addr

			// find the first entry not less than addr
			EntryMapType::iterator	iter = m_entries.lower_bound(addr);

			// iter is either equal to addr, greater than addr, or the end
			if(iter == m_entries.end())
			{
				// iter is the end
				// can only be in the entry before this
				// which does exist because map isn't empty
				--iter;

				if(iter->second.Contains(addr, iter->first))
				{
					result = iter;
				}
			}
			// at this point iter must be valid
			else if(iter->first > addr)
			{
				// iter is greater than addr
				// can only be in the entry before this
				// but there may not be an entry before this

				if(iter != m_entries.begin())
				{
					--iter;

					if(iter->second.Contains(addr, iter->first))
					{
						result = iter;
					}
				}
			}
			else
			{
				// iter is equal to addr and matches
				result = iter;
			}
		}

		return result;
	}

	/// First range in ascending start order (for iteration).
	typename EntryMapType::iterator	Begin(void)
	{
		return m_entries.begin();
	}

	/// One-past-the-last iterator (also LookupIter's "not found").
	typename EntryMapType::iterator	End(void)
	{
		return m_entries.end();
	}

private:
	EntryMapType	m_entries;
};

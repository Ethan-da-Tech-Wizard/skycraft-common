// ==============================================================================
// Program: IMemPool.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Fixed-size chunk memory pool allocator header for high-frequency allocations.
//          Inputs: Block size and block count specifications.
//          Outputs: Fast O(1) memory chunk pointers without heap lock contention.
//          Architecture: Prevents CRT heap fragmentation during game frame updates.
// ==============================================================================


#pragma once

#include "common/ICriticalSection.h"

/**
 * @class IMemPool
 *
 * Fixed-capacity object pool with intrusive free and allocated lists. Storage is a plain
 * array inside the pool object, so allocation never touches the CRT heap: that makes
 * Allocate()/Free() deterministic and avoids fragmentation, which matters for objects that
 * are created and destroyed every frame in a game loop.
 *
 * Lifetime model: slots are raw bytes; Allocate() placement-constructs a T, Free() runs ~T().
 * Not thread-safe. @tparam T object type (needs a default constructor). @tparam size slot
 * count, must be at least 1 (Reset() indexes slot size-1).
 */
// IMemPool<T, size>: a fixed-capacity pool of T objects that lives inside the
// pool object itself (no heap use). Unused slots form a free list; slots in use
// form an "allocated" list so the pool can be iterated and cleared. Objects are
// constructed in place with placement new on Allocate() and destroyed on Free().
// Not thread-safe (see IThreadSafeBasicMemPool). `size` must be >= 1.
template <typename T, UInt32 size>
// ---------------------------------------------------------------------------
// IMemPool: Pre-allocated memory pool managing free-lists of fixed-size blocks
// ---------------------------------------------------------------------------
class IMemPool
{
public:
	/// Build the pool with every slot free (Reset also initialises m_alloc).
	IMemPool()
	:m_free(NULL), m_alloc(NULL)
	{
		Reset();
	}

	/// Destroys any objects still allocated so their destructors run.
	~IMemPool()	{ Clear(); }

	/**
	 * Chain every slot into the free list (slot i points at slot i+1) and mark
	 * nothing as allocated. NOTE: this does not run destructors of live objects.
	 */
	// Chain every slot into the free list (slot i points at slot i+1) and mark
	// nothing as allocated. NOTE: this does not run destructors of live objects.
	void	Reset(void)
	{
		for(UInt32 i = 0; i < size - 1; i++)
		{
			m_items[i].next = &m_items[i + 1];
		}

		m_items[size - 1].next = NULL;
		m_free = m_items;
		m_alloc = NULL;
	}

	/**
	 * Pop a slot from the free list, push it on the allocated list, construct a T in
	 * it with its default constructor and return it.
	 *
	 * @return The new object, or NULL when the pool is full (callers must check).
	 */
	// Pop a slot from the free list, push it on the allocated list, construct a T in
	// it with its default constructor and return it; returns NULL when the pool is
	// full (callers must check).
	T *		Allocate(void)
	{
		if(m_free)
		{
			PoolItem	* item = m_free;
			m_free = m_free->next;

			item->next = m_alloc;
			m_alloc = item;

			T	* obj = item->GetObj();

			new (obj) T;
			return obj;
		}

		return NULL;
	}

	/**
	 * Return an object to the pool and run its destructor.
	 *
	 * The allocated list is singly linked, so this is O(n) in the number of live objects.
	 *
	 * @param obj Must have come from this pool and not already been freed (otherwise the
	 *            search below runs off the end of the list).
	 */
	// Unlink the slot from the allocated list. The list is singly linked, so this
	// is O(n) in the number of live objects. `obj` must have come from this pool and
	// not already been freed (otherwise the search below runs off the list).
	void	Free(T * obj)
	{
		// obj is the first member of PoolItem, so the object pointer is also the slot pointer.
		PoolItem	* item = reinterpret_cast <PoolItem *>(obj);

		// Head of the allocated list: just advance the head.
		if(item == m_alloc)
		{
			m_alloc = item->next;
		}
		// Otherwise find the predecessor of the slot and splice it out.
		else
		{
			PoolItem	* traverse = m_alloc;
			while(traverse->next != item)
				traverse = traverse->next;
			traverse->next = traverse->next->next;
		}

		// Push the slot on the free list before destroying the object; the slot's `next` field is
		// outside the object bytes, so the destructor does not disturb it.
		item->next = m_free;
		m_free = item;

		obj->~T();
	}

	/// @return Total slot count (capacity), not the number currently in use.
	UInt32	GetSize(void)	{ return size; }

	/**
	 * Iteration over live objects: Begin() returns the most recently allocated one
	 * and Next() walks toward the oldest.
	 *
	 * @return First live object, or NULL when the pool is empty.
	 */
	// Iteration over live objects: Begin() returns the most recently allocated one
	// and Next() walks toward the oldest. Returns NULL at the end.
	T *		Begin(void)
	{
		T	* result = NULL;

		if(m_alloc)
			result = m_alloc->GetObj();
		
		return result;
	}

	/**
	 * @param obj A live object previously returned by Begin()/Next().
	 * @return The next-older live object, or NULL at the end.
	 */
	T *		Next(T * obj)
	{
		PoolItem	* item = reinterpret_cast <PoolItem *>(obj);
		PoolItem	* next = item->next;
		T			* result = NULL;

		if(next)
			result = next->GetObj();

		return result;
	}

	/**
	 * Debug helper: logs the addresses on the free and allocated lists through the
	 * global log (gLog), indented by list.
	 */
	// Debug helper: logs the addresses on the free and allocated lists through the
	// global log (gLog).
	void	Dump(void)
	{
		gLog.Indent();

		_DMESSAGE("free:");
		gLog.Indent();
		for(PoolItem * traverse = m_free; traverse; traverse = traverse->next)
			_DMESSAGE("%08X", traverse);
		gLog.Outdent();

		_DMESSAGE("alloc:");
		gLog.Indent();
		for(PoolItem * traverse = m_alloc; traverse; traverse = traverse->next)
			_DMESSAGE("%08X", traverse);
		gLog.Outdent();

		gLog.Outdent();
	}

	/// @return true when no slot is free (the next Allocate() would return NULL).
	bool	Full(void)
	{
		return m_free == NULL;
	}

	/// @return true when no object is allocated.
	bool	Empty(void)
	{
		return m_alloc == NULL;
	}

	/// Frees (and destroys) every live object.
	// Frees (and destroys) every live object.
	void	Clear(void)
	{
		while(m_alloc)
			Free(m_alloc->GetObj());
	}

// Slots are intentionally kept private; callers only see T pointers.
private:
	/**
	 * Storage for one object plus its list link. The raw bytes are kept as UInt8[]
	 * so T is not constructed until Allocate(); reinterpret_cast converts between the
	 * slot and the object because the object is the first member.
	 */
	// Storage for one object plus its list link. The raw bytes are kept as UInt8[]
	// so T is not constructed until Allocate(); reinterpret_cast converts between the
	// slot and the object because the object is the first member.
	struct PoolItem
	{
		UInt8		obj[sizeof(T)];
		PoolItem	* next;

		T *			GetObj(void)	{ return reinterpret_cast <T *>(obj); }
	};

	// m_items: the pool storage. m_free / m_alloc: heads of the two singly linked lists.
	PoolItem	m_items[size];
	PoolItem	* m_free;
	PoolItem	* m_alloc;
};

/**
 * @class IBasicMemPool
 *
 * Lighter variant with only a free list. Slots are a
 * union of the object bytes and the free-list pointer, so there is no per-slot
 * overhead while in use, but the pool cannot enumerate live objects and Reset()
 * and the destructor do not destroy them. Not thread-safe.
 */
// IBasicMemPool<T, size>: lighter variant with only a free list. Slots are a
// union of the object bytes and the free-list pointer, so there is no per-slot
// overhead while in use, but the pool cannot enumerate live objects and Reset()
// and the destructor do not destroy them.
template <typename T, UInt32 size>
class IBasicMemPool
{
public:
	/// Build the pool with every slot free.
	IBasicMemPool()
	:m_free(NULL)
	{
		Reset();
	}

	/// Does not destroy live objects; the owner must Free() them first if their destructors matter.
	~IBasicMemPool()	{ }

	/// Rebuild the free list over all slots (live objects are forgotten, not destroyed).
	void	Reset(void)
	{
		for(UInt32 i = 0; i < size - 1; i++)
		{
			m_items[i].next = &m_items[i + 1];
		}

		m_items[size - 1].next = NULL;
		m_free = m_items;
	}

	/// @return A default-constructed T from the free list, or NULL when the pool is full.
	T *		Allocate(void)
	{
		if(m_free)
		{
			PoolItem	* item = m_free;
			m_free = m_free->next;

			T	* obj = item->GetObj();

			new (obj) T;
			return obj;
		}

		return NULL;
	}

	/**
	 * O(1): destroy the object, then push its slot on the free list.
	 *
	 * @param obj Object from this pool, not already freed (a double free corrupts the free list).
	 */
	// O(1): destroy the object, then push its slot on the free list.
	void	Free(T * obj)
	{
		obj->~T();

		PoolItem	* item = reinterpret_cast <PoolItem *>(obj);

		item->next = m_free;
		m_free = item;
	}

	/// @return Total slot count (capacity).
	UInt32	GetSize(void)	{ return size; }

	/// @return true when no slot is free.
	bool	Full(void)
	{
		return m_free == NULL;
	}

	/**
	 * Slot index of an object (pointer difference from the first slot), useful as a
	 * compact handle; GetByID() converts back.
	 */
	// Slot index of an object (pointer difference from the first slot), useful as a
	// compact handle; GetByID() converts back.
	UInt32	GetIdx(T * obj)
	{
		PoolItem	* item = reinterpret_cast <PoolItem *>(obj);

		return item - m_items;
	}

	/// @param id Slot index from GetIdx(); not range-checked. @return The object at that slot (valid only while allocated).
	T *		GetByID(UInt32 id)
	{
		return m_items[id].GetObj();
	}

// In-use slot: object bytes. Free slot: the link to the next free slot (the two never coexist, hence the union).
private:
	union PoolItem
	{
		UInt8		obj[sizeof(T)];
		PoolItem	* next;

		T *			GetObj(void)	{ return reinterpret_cast <T *>(obj); }
	};

	PoolItem	m_items[size];
	PoolItem	* m_free;
};

/**
 * @class IThreadSafeBasicMemPool
 *
 * IBasicMemPool guarded by a critical section
 * around free-list updates. The lock is released before the constructor runs so
 * object construction does not happen inside the critical section.
 */
// IThreadSafeBasicMemPool<T, size>: IBasicMemPool guarded by a critical section
// around free-list updates. The lock is released before the constructor runs so
// object construction does not happen inside the critical section.
template <typename T, UInt32 size>
class IThreadSafeBasicMemPool
{
public:
	/// Build the pool with every slot free.
	IThreadSafeBasicMemPool()
	:m_free(NULL)
	{
		Reset();
	}

	~IThreadSafeBasicMemPool()	{ }

	/// Rebuild the free list under the lock; live objects are forgotten, not destroyed.
	void	Reset(void)
	{
		m_mutex.Enter();

		for(UInt32 i = 0; i < size - 1; i++)
		{
			m_items[i].next = &m_items[i + 1];
		}

		m_items[size - 1].next = NULL;
		m_free = m_items;

		m_mutex.Leave();
	}

	/// @return A default-constructed T, or NULL when the pool is full. Safe to call from several threads.
	T *		Allocate(void)
	{
		T	* result = NULL;

		// Pop under the lock; construct after unlocking (see the class comment). Both branches must
		// Leave() the mutex exactly once.
		m_mutex.Enter();

		if(m_free)
		{
			PoolItem	* item = m_free;
			m_free = m_free->next;

			m_mutex.Leave();

			result = item->GetObj();

			new (result) T;
		}
		else
		{
			m_mutex.Leave();
		}

		return result;
	}

	/// Destroy the object first (outside the lock), then push its slot back under the lock.
	void	Free(T * obj)
	{
		obj->~T();

		PoolItem	* item = reinterpret_cast <PoolItem *>(obj);

		m_mutex.Enter();

		item->next = m_free;
		m_free = item;

		m_mutex.Leave();
	}

	/// @return Total slot count (capacity).
	UInt32	GetSize(void)	{ return size; }

	/// @return true when no slot is free (value may be stale by the time the caller acts on it).
	bool	Full(void)
	{
		return m_free == NULL;
	}

// Same slot layout as IBasicMemPool; m_mutex protects only m_free.
private:
	union PoolItem
	{
		UInt8		obj[sizeof(T)];
		PoolItem	* next;

		T *			GetObj(void)	{ return reinterpret_cast <T *>(obj); }
	};

	PoolItem	m_items[size];
	PoolItem	* m_free;

	ICriticalSection	m_mutex;
};

/// Self-test entry point (implemented in IMemPool.cpp); output goes to the debug log.
// Self-test entry point (implemented in IMemPool.cpp).
void Test_IMemPool(void);

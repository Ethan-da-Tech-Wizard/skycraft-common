// ==============================================================================
// Program: ILinkedList.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Template implementation of intrusive singly and doubly linked list containers.
//          Inputs: Node pointers and value items.
//          Outputs: O(1) node insertion, deletion, and bidirectional traversal.
//          Architecture: Zero-allocation node linking for SKSE64 internal hook chains.
// ==============================================================================


#pragma once

/**
 * @struct ILink
 *
 * Intrusive list support: a type that wants to live in a list embeds an ILink<T>
 * member. The ILink holds next/prev pointers; GetObj()/GetLink() convert between
 * the link and the containing object using the member's byte offset
 * (ILink<T>::s_offset, defined with the ILINK_INIT macro). No allocation is done.
 * ILink members must be public. Because s_offset is one value per T, an object type can embed only one ILink<T> member.
 */
template <typename T>
struct ILink
{
	/// Byte offset of the ILink member inside T; defined once per type through ILINK_INIT.
	static const UInt32 s_offset;

	ILink <T>	* next;
	ILink <T>	* prev;

	/// @return The containing object, found by subtracting the member offset from this link's address.
	T *	GetObj(void)	{ return (T *)(((uintptr_t)this) - s_offset); }

	/// @param obj Object that embeds an ILink<T>. @return Its link (object address plus member offset).
	static ILink <T> *	GetLink(T * obj)	{ return (ILink <T> *)(((uintptr_t)obj) + s_offset); }

	/**
	 * Remove this link from its neighbours and clear its own pointers.
	 * The neighbours' pointers are repaired, but any list head that pointed at this link is not updated by this method.
	 */
	void	Unlink(void)
	{
		if(next)	next->prev = prev;
		if(prev)	prev->next = next;

		next = prev = NULL;
	}

	/// Convenience: insert the link embedded in `obj` immediately before this one.
	void	LinkBefore(T * obj)
	{
		LinkBefore(GetLink(obj));
	}

	/// Convenience: insert the link embedded in `obj` immediately after this one.
	void	LinkAfter(T * obj)
	{
		LinkAfter(GetLink(obj));
	}

	/**
	 * Insert `link` immediately before this one.
	 * @param link Unlinked link to insert; its next/prev are overwritten.
	 */
	void	LinkBefore(ILink <T> * link)
	{
		link->next = this;
		link->prev = prev;

		// Fix the old predecessor's forward pointer; when this was the first element prev is NULL and there is nothing to fix.
		if(prev)
		{
			prev->next = link;
		}

		prev = link;
	}

	/**
	 * Insert `link` immediately after this one.
	 * @param link Unlinked link to insert; its next/prev are overwritten.
	 */
	void	LinkAfter(ILink <T> * link)
	{
		link->next = next;
		link->prev = this;

		// Fix the old successor's back pointer, if any.
		if(next)
		{
			next->prev = link;
		}

		next = link;
	}
};

/**
 * @struct ILinkedList
 *
 * Doubly linked list with two sentinel links (begin and end) so insertion never
 * has to special-case an empty list. Call Reset() before use. Real elements sit between the sentinels.
 */
template <typename T>

struct ILinkedList
{
	// Sentinels: begin.prev and end.next stay NULL; iterate from begin.next until reaching &end.
	ILink <T>	begin;
	ILink <T>	end;

	/// Make the list empty: begin and end point at each other.
	void	Reset(void)
	{
		begin.next = &end;
		begin.prev = NULL;
		end.next = NULL;
		end.prev = &begin;
	}

	/**
	 * Insert right after the begin sentinel (becomes the first element).
	 * @param obj Object embedding an ILink<T>, not already in a list.
	 */
	void	PushFront(T * obj)
	{
		ILink <T>	* objLink = ILink <T>::GetLink(obj);

		objLink->next = begin.next;
		objLink->prev = &begin;

		if(objLink->next)
		{
			objLink->next->prev = objLink;
		}

		begin.next = objLink;
	}
};

/**
 * Defines the static offset for ILink<T>: pass the containing type and the name
 * of its ILink member, once per type, in a .cpp file.
 * Example: ILINK_INIT(MyThing, link); for a MyThing that has a public `ILink<MyThing> link;`.
 */
#define ILINK_INIT(baseType, memberName)	template <typename T> const UInt32 ILink <T>::s_offset = offsetof(baseType, memberName)

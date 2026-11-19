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

// Intrusive list support: a type that wants to live in a list embeds an ILink<T>
// member. The ILink holds next/prev pointers; GetObj()/GetLink() convert between
// the link and the containing object using the member's byte offset
// (ILink<T>::s_offset, defined with the ILINK_INIT macro). No allocation is done.
// ILink members must be public
template <typename T>
struct ILink
{
	static const UInt32 s_offset;

	ILink <T>	* next;
	ILink <T>	* prev;

	T *	GetObj(void)	{ return (T *)(((uintptr_t)this) - s_offset); }

	static ILink <T> *	GetLink(T * obj)	{ return (ILink <T> *)(((uintptr_t)obj) + s_offset); }

	// Remove this link from its neighbours and clear its own pointers.
	void	Unlink(void)
	{
		if(next)	next->prev = prev;
		if(prev)	prev->next = next;

		next = prev = NULL;
	}

	void	LinkBefore(T * obj)
	{
		LinkBefore(GetLink(obj));
	}

	void	LinkAfter(T * obj)
	{
		LinkAfter(GetLink(obj));
	}

	// Insert `link` immediately before this one.
	void	LinkBefore(ILink <T> * link)
	{
		link->next = this;
		link->prev = prev;

		if(prev)
		{
			prev->next = link;
		}

		prev = link;
	}

	// Insert `link` immediately after this one.
	void	LinkAfter(ILink <T> * link)
	{
		link->next = next;
		link->prev = this;

		if(next)
		{
			next->prev = link;
		}

		next = link;
	}
};

template <typename T>
// Doubly linked list with two sentinel links (begin and end) so insertion never
// has to special-case an empty list. Call Reset() before use.
struct ILinkedList
{
	ILink <T>	begin;
	ILink <T>	end;

	void	Reset(void)
	{
		begin.next = &end;
		begin.prev = NULL;
		end.next = NULL;
		end.prev = &begin;
	}

	// Insert right after the begin sentinel.
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

// Defines the static offset for ILink<T>: pass the containing type and the name
// of its ILink member, once per type, in a .cpp file.
#define ILINK_INIT(baseType, memberName)	template <typename T> const UInt32 ILink <T>::s_offset = offsetof(baseType, memberName)

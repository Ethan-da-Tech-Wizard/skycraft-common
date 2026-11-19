// ==============================================================================
// Program: ICriticalSection.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Header specification for lightweight user-mode critical section synchronization.
//          Inputs: Win32 CRITICAL_SECTION objects and thread acquisition requests.
//          Outputs: Mutual exclusion guards preventing data races in multi-threaded runtime.
//          Design Rationale: Ultra-fast spin-lock/wait hybrid for game thread safety.
// ==============================================================================

#pragma once

/**
 * @class ICriticalSection
 *
 * Wrapper around a Win32 CRITICAL_SECTION: a lightweight, process-local mutex that the same thread may
 * enter repeatedly (recursive; each Enter needs a matching Leave). Initialised in the constructor and
 * deleted in the destructor, so it must outlive every thread using it. Not copyable in practice (copying a CRITICAL_SECTION is undefined).
 */
// ---------------------------------------------------------------------------
// ICriticalSection: RAII synchronization wrapper for Win32 CRITICAL_SECTION
// ---------------------------------------------------------------------------
class ICriticalSection
{
	public:
		/// Initialise the underlying critical section.
		ICriticalSection()	{ InitializeCriticalSection(&critSection); }
		/// Destroy it; no thread may be inside it.
		~ICriticalSection()	{ DeleteCriticalSection(&critSection); }

		/// Block until the calling thread owns the section.
		void	Enter(void)		{ EnterCriticalSection(&critSection); }
		/// Release one level of ownership.
		void	Leave(void)		{ LeaveCriticalSection(&critSection); }
		/// @return true if ownership was obtained without blocking; false if another thread holds it.
		bool	TryEnter(void)	{ return TryEnterCriticalSection(&critSection) != 0; }

	private:
		CRITICAL_SECTION	critSection;
};

/**
 * @class IScopedCriticalSection
 *
 * RAII guard: enters the given critical section in its constructor and leaves it in its destructor,
 * so every return path (including exceptions) releases the lock.
 * Usage: `IScopedCriticalSection lock(&cs);` at the top of a block.
 */
class IScopedCriticalSection
{
public:
	/// @param cs Section to lock for this object's lifetime (must be non-NULL and outlive the guard).
	IScopedCriticalSection(ICriticalSection * cs)
		:m_cs(cs)
	{
		m_cs->Enter();
	}

	~IScopedCriticalSection()
	{
		m_cs->Leave();
	}

private:
	// Private and undefined: a guard must always be given a section.
	IScopedCriticalSection(); // undefined

	ICriticalSection * m_cs;
};

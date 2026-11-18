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

// ---------------------------------------------------------------------------
// ICriticalSection: RAII synchronization wrapper for Win32 CRITICAL_SECTION
// ---------------------------------------------------------------------------
class ICriticalSection
{
	public:
		ICriticalSection()	{ InitializeCriticalSection(&critSection); }
		~ICriticalSection()	{ DeleteCriticalSection(&critSection); }

		void	Enter(void)		{ EnterCriticalSection(&critSection); }
		void	Leave(void)		{ LeaveCriticalSection(&critSection); }
		bool	TryEnter(void)	{ return TryEnterCriticalSection(&critSection) != 0; }

	private:
		CRITICAL_SECTION	critSection;
};

class IScopedCriticalSection
{
public:
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
	IScopedCriticalSection(); // undefined

	ICriticalSection * m_cs;
};

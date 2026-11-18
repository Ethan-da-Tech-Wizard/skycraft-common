// ==============================================================================
// Program: IDirectoryIterator.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of Win32 directory scanning and file metadata extraction.
//          Inputs: Wildcard path specifications (e.g., Data/SKSE/Plugins/*.dll).
//          Outputs: Sequential file name enumeration and directory status.
//          Algorithm: Win32 FindFirstFile/FindNextFile cursor iteration.
// ==============================================================================

#include "IDirectoryIterator.h"
#include <string>

// ---------------------------------------------------------------------------
// Constructor: Prepares iterator state and invalidates handle
// ---------------------------------------------------------------------------
IDirectoryIterator::IDirectoryIterator(const char * path, const char * match)
:m_searchHandle(INVALID_HANDLE_VALUE), m_done(false)
{
	if(!match) match = "*";

	strcpy_s(m_path, sizeof(m_path), path);

	char	wildcardPath[MAX_PATH];
	sprintf_s(wildcardPath, sizeof(wildcardPath), "%s\\%s", path, match);

	m_searchHandle = FindFirstFile(wildcardPath, &m_result);
	if(m_searchHandle == INVALID_HANDLE_VALUE)
		m_done = true;
}

IDirectoryIterator::~IDirectoryIterator()
{
	if(m_searchHandle != INVALID_HANDLE_VALUE)
		FindClose(m_searchHandle);
}

void IDirectoryIterator::GetFullPath(char * out, UInt32 outLen)
{
	sprintf_s(out, outLen, "%s\\%s", m_path, m_result.cFileName);
}

std::string IDirectoryIterator::GetFullPath(void)
{
	return std::string(m_path) + "\\" + std::string(m_result.cFileName);
}

void IDirectoryIterator::Next(void)
{
	BOOL	result = FindNextFile(m_searchHandle, &m_result);
	if(!result)
		m_done = true;
}

bool IDirectoryIterator::Done(void)
{
	return m_done;
}

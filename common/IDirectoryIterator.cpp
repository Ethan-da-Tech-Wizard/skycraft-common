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
/*
 * Thin wrapper over Win32 FindFirstFile/FindNextFile. Usage:
 *   for(IDirectoryIterator it("C:\\dir", "*.dll"); !it.Done(); it.Next()) { ... it.Get() / it.GetFullPath() ... }
 * It lists one directory (not recursive) and includes "." and ".." entries and sub-folders
 * unless the pattern excludes them. m_result always holds the CURRENT entry.
 */


/**
 * Start a directory listing and position on the first match.
 *
 * @param path  Directory to list, without a trailing slash (a backslash is added).
 * @param match Wildcard such as "*.dll"; NULL means everything.
 *
 * If nothing matches or the directory is missing, Done() is true immediately.
 * NOTE: m_path is a fixed-size buffer; strcpy_s aborts via the CRT invalid-parameter handler on an over-long path.
 */
// ---------------------------------------------------------------------------
// Constructor: Prepares iterator state and invalidates handle
// ---------------------------------------------------------------------------
IDirectoryIterator::IDirectoryIterator(const char * path, const char * match)
:m_searchHandle(INVALID_HANDLE_VALUE), m_done(false)
{
	// Default to every entry.
	if(!match) match = "*";

	// Keep the directory so GetFullPath() can rebuild the full path of each entry.
	strcpy_s(m_path, sizeof(m_path), path);

	char	wildcardPath[MAX_PATH];
	sprintf_s(wildcardPath, sizeof(wildcardPath), "%s\\%s", path, match);

	// FindFirstFile both opens the search and returns the first entry; failure means "no entries".
	m_searchHandle = FindFirstFile(wildcardPath, &m_result);
	if(m_searchHandle == INVALID_HANDLE_VALUE)
		m_done = true;
}

/// Release the search handle if one is open.
IDirectoryIterator::~IDirectoryIterator()
{
	if(m_searchHandle != INVALID_HANDLE_VALUE)
		FindClose(m_searchHandle);
}

/**
 * Build the full path of the current entry into a caller-supplied buffer.
 *
 * @param out    Destination buffer.
 * @param outLen Buffer size in bytes; sprintf_s guards against overflow.
 */
void IDirectoryIterator::GetFullPath(char * out, UInt32 outLen)
{
	sprintf_s(out, outLen, "%s\\%s", m_path, m_result.cFileName);
}

/// Same as above but returns a std::string (path + backslash + file name).
std::string IDirectoryIterator::GetFullPath(void)
{
	return std::string(m_path) + "\\" + std::string(m_result.cFileName);
}

/// Advance to the next entry; sets Done() when the listing is exhausted.
void IDirectoryIterator::Next(void)
{
	BOOL	result = FindNextFile(m_searchHandle, &m_result);
	if(!result)
		m_done = true;
}

/// @return true when there is no current entry (before use: nothing matched; after Next: finished).
bool IDirectoryIterator::Done(void)
{
	return m_done;
}

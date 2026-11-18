// ==============================================================================
// Program: IDirectoryIterator.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Filesystem directory enumeration header for plugin and asset discovery.
//          Inputs: Search wildcards and directory path strings.
//          Outputs: File attributes, names, sizes, and directory traversal tokens.
//          Architecture: SKSE64 plugin scanner locating runtime DLL extensions.
// ==============================================================================

#pragma once

// ---------------------------------------------------------------------------
// IDirectoryIterator: Encapsulates Win32 FindFirstFile/FindNextFile handles
// ---------------------------------------------------------------------------
class IDirectoryIterator
{
public:
	IDirectoryIterator(const char * path, const char * match = NULL);
	virtual ~IDirectoryIterator();

	WIN32_FIND_DATA *	Get(void)	{ return &m_result; }
	void		GetFullPath(char * out, UInt32 outLen);
	std::string	GetFullPath(void);

	void	Next(void);
	bool	Done(void);

private:
	IDirectoryIterator();	// undefined, disallow

	HANDLE			m_searchHandle;
	WIN32_FIND_DATA	m_result;
	bool			m_done;

	char	m_path[MAX_PATH];
};

// ==============================================================================
// Program: IFileStream.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of Win32 CreateFile, ReadFile, WriteFile, and directory creation.
//          Inputs: File paths and binary memory buffers.
//          Outputs: Synchronous disk writes and sector-aligned file reads.
//          Algorithm: Win32 SetFilePointer and recursive MakeAllDirs path creation.
// ==============================================================================


#include "IFileStream.h"
#include "IDebugLog.h"
/*
 * IFileStream: IDataStream implementation over a Win32 file HANDLE. A stream is either
 * open for reading (Open) or for writing (Create), never both: Open requests GENERIC_READ
 * with shared read access and Create requests GENERIC_WRITE exclusively. streamOffset /
 * streamLength (from IDataStream) mirror the file pointer and size so queries do not need a system call.
 * Windows only (Win32 file and common-dialog APIs); it does not build on other platforms.
 */

#include "IErrors.h"
#include <direct.h>

/// Create a stream with no file attached (INVALID_HANDLE_VALUE means "closed").
IFileStream::IFileStream()
:theFile(INVALID_HANDLE_VALUE)
{
	
}

/// Convenience constructor that opens `name` for reading. The result of Open() is discarded, so callers that need to know whether it worked should use the default constructor and call Open() themselves.
IFileStream::IFileStream(const char * name)
:theFile(INVALID_HANDLE_VALUE)
{
	Open(name);
}

/// Closes the file handle, if any.
IFileStream::~IFileStream()
{
	Close();
}

/**
 * Open an existing file for reading and attach it to the stream (closing any previous file).
 *
 * @param name Path of the file.
 * @return true if opened; false if it does not exist or cannot be read.
 *
 * On success streamLength is set from the file size and the offset starts at 0.
 */
/**
 *	Opens a file for reading and attaches it to the stream
 */
// ---------------------------------------------------------------------------
// Open: Opens existing file for reading using Win32 CreateFile
// ---------------------------------------------------------------------------
bool IFileStream::Open(const char * name)
{
	Close();

	theFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(theFile != INVALID_HANDLE_VALUE)
	{
		// 64-bit size query so files over 4 GiB are measured correctly.
		LARGE_INTEGER	temp;

		GetFileSizeEx(theFile, &temp);

		streamLength = temp.QuadPart;
		streamOffset = 0;
	}

	return theFile != INVALID_HANDLE_VALUE;
}

/// Hook procedure for the file dialogs. Present only because OFN_ENABLEHOOK selects the Explorer-style dialog layout we want with a hook; it handles no messages (returns 0 = default processing).
static UINT_PTR CALLBACK BrowseEventProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/**
 * Show a "file open" dialog and open the chosen file for reading.
 *
 * @return true if the user picked a file and it opened; false if the dialog was cancelled or the open failed.
 *
 * The no-change-dir flag keeps the process working directory from moving, which would break later relative-path loads.
 * NOTE: only the fields assigned below are set; the OPENFILENAME struct is not zeroed first.
 */
bool IFileStream::BrowseOpen(void)
{
	bool			result = false;
	OPENFILENAME	info;
	char			path[4096];

	path[0] = 0;

	info.lStructSize =			sizeof(info);
	info.hwndOwner =			NULL;
	info.hInstance =			NULL;
	info.lpstrFilter =			NULL;
	info.lpstrCustomFilter =	NULL;
	info.nMaxCustFilter =		0;
	info.nFilterIndex =			0;
	info.lpstrFile =			path;
	info.nMaxFile =				sizeof(path);
	info.lpstrFileTitle =		NULL;
	info.nMaxFileTitle =		0;
	info.lpstrInitialDir =		NULL;
	info.lpstrTitle =			NULL;
	info.Flags =				OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_ENABLEHOOK | OFN_NOCHANGEDIR;
	info.lpstrDefExt =			NULL;
	info.lCustData =			NULL;
	info.lpfnHook =				BrowseEventProc;
	info.lpTemplateName =		NULL;
// The remaining OPENFILENAME fields only exist in newer SDK versions of the struct, so they are left out to stay compatible with old compilers.
//	info.pvReserved =			NULL;
//	info.dwReserved =			NULL;
//	info.FlagsEx =				0;

	if(GetOpenFileName(&info))
	{
		result = Open(path);
	}

	return result;
}

/**
 * Create a new file for writing, overwriting any previously-existing file,
 * and attach it to the stream.
 *
 * @param name Path of the file. Parent directories must already exist (see MakeAllDirs).
 * @return true if created; false on failure (locked file, bad path, access denied).
 */
/**
 *	Creates a new file for writing, overwriting any previously-existing files,
 *	and attaches it to the stream
 */
// ---------------------------------------------------------------------------
// Create: Creates or truncates destination file for binary writing
// ---------------------------------------------------------------------------
bool IFileStream::Create(const char * name)
{
	Close();

	// CREATE_ALWAYS truncates an existing file; share mode 0 keeps other processes out while we write.
	theFile = CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(theFile != INVALID_HANDLE_VALUE)
	{
		streamLength = 0;
		streamOffset = 0;
	}

	return theFile != INVALID_HANDLE_VALUE;
}

/**
 * Show a "save as" dialog and create the chosen file for writing.
 *
 * @param defaultName Initial file name shown in the dialog, or NULL.
 * @param defaultPath Initial folder, or NULL for the system default.
 * @param title       Dialog title, or NULL.
 * @return true if the user chose a file and it was created; false on cancel or failure.
 *
 * Asks the user before overwriting and requires the folder to exist.
 * NOTE: if defaultName is NULL the `path` buffer is passed uninitialised.
 */
bool IFileStream::BrowseCreate(const char * defaultName, const char * defaultPath, const char * title)
{
	bool			result = false;
	OPENFILENAME	info;
	char			path[4096];

	// Seed the dialog's file-name box (the buffer also receives the final chosen path).
	if(defaultName)
		strcpy_s(path, sizeof(path), defaultName);

	info.lStructSize =			sizeof(info);
	info.hwndOwner =			NULL;
	info.hInstance =			NULL;
	info.lpstrFilter =			NULL;
	info.lpstrCustomFilter =	NULL;
	info.nMaxCustFilter =		0;
	info.nFilterIndex =			0;
	info.lpstrFile =			path;
	info.nMaxFile =				sizeof(path);
	info.lpstrFileTitle =		NULL;
	info.nMaxFileTitle =		0;
	info.lpstrInitialDir =		defaultPath;
	info.lpstrTitle =			title;
	info.Flags =				OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK |
								OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	info.lpstrDefExt =			NULL;
	info.lCustData =			NULL;
	info.lpfnHook =				BrowseEventProc;
	info.lpTemplateName =		NULL;
//	info.pvReserved =			NULL;
//	info.dwReserved =			NULL;
//	info.FlagsEx =				0;

	if(GetSaveFileName(&info))
	{
		result = Create(path);
	}

	return result;
}

/**
 * Close the current file. Safe to call repeatedly.
 * NOTE: the guard `if(theFile)` is also true for INVALID_HANDLE_VALUE (-1), so a second Close calls CloseHandle on an invalid handle, which fails harmlessly.
 */
/**
 *	Closes the current file
 */
void IFileStream::Close(void)
{
	if(theFile)
	{
		CloseHandle(theFile);
		theFile = INVALID_HANDLE_VALUE;
	}
}

/**
 * Read bytes at the current position into buf and advance streamOffset by the bytes actually read.
 *
 * @param buf      Destination, at least inLength bytes.
 * @param inLength Number of bytes requested.
 * A short read at end of file is not reported; callers should check HitEOF() / GetRemain() first.
 */
void IFileStream::ReadBuf(void * buf, UInt32 inLength)
{
	UInt32	bytesRead;

	ReadFile(theFile, buf, inLength, &bytesRead, NULL);

	streamOffset += bytesRead;
}

/**
 * Write bytes at the current position, extending the file when writing past its end.
 *
 * @param buf      Source data.
 * @param inLength Number of bytes.
 */
void IFileStream::WriteBuf(const void * buf, UInt32 inLength)
{
	UInt32	bytesWritten;

	// check for file expansion: if the caller seeked beyond the end, make the file that long first so the gap is defined, not left to the OS
	// check for file expansion
	if(streamOffset > streamLength)
		SetEndOfFile(theFile);

	WriteFile(theFile, buf, inLength, &bytesWritten, NULL);

	// Keep the cached position and length in sync with what the OS did.
	streamOffset += bytesWritten;

	if(streamLength < streamOffset)
		streamLength = streamOffset;
}

/**
 * Move the file pointer.
 *
 * @param inOffset Absolute offset from the start of the file, in bytes (64-bit).
 */
void IFileStream::SetOffset(SInt64 inOffset)
{
	LARGE_INTEGER	temp;

	temp.QuadPart = inOffset;

	SetFilePointerEx(theFile, temp, NULL, FILE_BEGIN);
	streamOffset = inOffset;
}

/**
 * Truncate or extend the file to exactly `length` bytes, leaving the offset at the end.
 *
 * @param length New size in bytes.
 */
void IFileStream::SetLength(UInt64 length)
{
	SetOffset(length);
	SetEndOfFile(theFile);

	streamLength = length;
}

/**
 * Create every folder in a path, like `mkdir -p`.
 *
 * @param path Path ending in a file name or a trailing slash; the final component after the
 *             last slash is NOT created (so "a/b/file.txt" makes a and a/b).
 *
 * Each separator ends a prefix, which is handed to _mkdir; failures (already exists) are ignored.
 * ### TODO (original): get rid of the fixed 1024-byte buf; longer paths overflow it.
 */
// ### TODO: get rid of buf
void IFileStream::MakeAllDirs(const char * path)
{
	char	buf[1024];
	char	* traverse = buf;

	while(1)
	{
		char	data = *path++;

		if(!data)
			break;

		if((data == '\\') || (data == '/'))
		{
			*traverse = 0;
			_mkdir(buf);
		}

		*traverse++ = data;
	}
}

/**
 * Find the file-name part of a path.
 *
 * @param path Path using '\\' or '/' separators.
 * @return Pointer INTO `path` just past the last separator, or NULL if the path contains no separator.
 *         Not a copy: the caller must keep `path` alive and must not free the result.
 */
char * IFileStream::ExtractFileName(char * path)
{
	char	* traverse = path;
	char	* lastSlash = NULL;

	while(1)
	{
		char	data = *traverse++;

		if((data == '\\') || (data == '/'))
			lastSlash = traverse;

		if(!data)
			break;
	}

	return lastSlash;
}

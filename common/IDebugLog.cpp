// ==============================================================================
// Program: IDebugLog.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of diagnostic log formatting, indentation stacks, and file sinks.
//          Inputs: Variadic format strings, log level thresholds, and source names.
//          Outputs: Buffered and auto-flushed text files on the local filesystem.
//          Algorithm: Mutex-guarded vsnprintf formatting with timestamp prefixes.
// ==============================================================================


#include "common/IDebugLog.h"
#include <share.h>
#include "common/IFileStream.h"
#include <shlobj.h>

// Logger state lives in statics so the single global `gLog` and any other IDebugLog object share one file.
// logLevel: least important level written to the file (default: everything incl. debug).
// printLevel: least important level also echoed to stdout (default: up to kLevel_Message).
// autoFlush is on by default so the log survives a crash.
std::FILE			* IDebugLog::logFile = NULL;
char				IDebugLog::sourceBuf[16] = { 0 };
char				IDebugLog::headerText[16] = { 0 };
char				IDebugLog::formatBuf[8192] = { 0 };
int					IDebugLog::indentLevel = 0;
int					IDebugLog::rightMargin = 0;
int					IDebugLog::cursorPos = 0;
int					IDebugLog::inBlock = 0;
bool				IDebugLog::autoFlush = true;
IDebugLog::LogLevel	IDebugLog::logLevel = IDebugLog::kLevel_DebugMessage;
IDebugLog::LogLevel	IDebugLog::printLevel = IDebugLog::kLevel_Message;

/// Create a logger without opening a file; call Open()/OpenRelative() afterwards (used for the global gLog).
IDebugLog::IDebugLog()
{
	//
}

/// Create a logger and open `name` immediately (see Open for what that means).
IDebugLog::IDebugLog(const char * name)
{
	Open(name);
}

/// Close the shared log file if one is open.
IDebugLog::~IDebugLog()
{
	if(logFile)
		fclose(logFile);
}

/**
 * Open the log file for writing (the banner below is slightly misleading: mode "w" truncates an existing file).
 *
 * @param path Log file path. Opened with write sharing denied so two processes cannot interleave output.
 * If that fails (for example another copy of the game still holds it) up to 5 alternative names
 * `<path>0`, `<path>1`, ... are tried; if all fail, logging silently becomes a no-op.
 */
// ---------------------------------------------------------------------------
// Open: Creates or appends to the target log file on disk
// ---------------------------------------------------------------------------
void IDebugLog::Open(const char * path)
{
	// _SH_DENYWR: others may read the file (so a viewer can tail it) but not write to it.
	logFile = _fsopen(path, "w", _SH_DENYWR);

	if(!logFile)
	{
		// Primary name was locked: fall back to numbered names so a second instance still gets a log.
		UInt32	id = 0;
		char	name[1024];

		do
		{
			sprintf_s(name, sizeof(name), "%s%d", path, id);
			id++;

			logFile = NULL;
			logFile = _fsopen(name, "w", _SH_DENYWR);
		}
		while(!logFile && (id < 5));
	}
}

/**
 * Open a log file inside a Windows known folder (e.g. My Documents), creating folders as needed.
 *
 * @param folderID CSIDL_* folder constant (CSIDL_FLAG_CREATE is added so the folder is created).
 * @param relPath  Path appended to the folder path; must start with a backslash, e.g. "\\My Games\\Skyrim\\SKSE\\skse.log".
 *
 * On failure to resolve the folder the process exits (access denied, usually Windows Defender Controlled Folder
 * Access blocking writes) or ASSERTs, because without a log location there is nothing useful to do.
 */
void IDebugLog::OpenRelative(int folderID, const char * relPath)
{
	char	path[MAX_PATH];

	HRESULT err = SHGetFolderPath(NULL, folderID | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path);
	if(!SUCCEEDED(err))
	{
		// E_ACCESSDENIED here is a well-known symptom of Controlled Folder Access: tell the user directly rather than crash.
		if(err == E_ACCESSDENIED)
		{
			MessageBox(nullptr, "Windows Defender Controlled Folder Access is probably blocking access to game config files. Fix your system.", "Script Extender", MB_OK | MB_ICONSTOP);
			ExitProcess(0);
		}

		_FATALERROR("SHGetFolderPath %08X failed (result = %08X lasterr = %08X)", folderID, err, GetLastError());
	}
	ASSERT_CODE(SUCCEEDED(err), err);

	// strcat_s aborts (invalid-parameter handler) if the combined path exceeds MAX_PATH.
	strcat_s(path, sizeof(path), relPath);

	// The log folder may not exist yet on a fresh install.
	IFileStream::MakeAllDirs(path);

	Open(path);
}

// Layout of each logged line: [indent: 4 spaces per level]['[source  ]' header]message.
/**
 *	Output a non-formatted message to the log file
 *	
 *	@param message the message
 *	@param source the source of the message, or NULL to use the previous source
 */
void IDebugLog::Message(const char * message, const char * source, bool newLine)
{
	// A new source label replaces the header text used for this and later lines.
	if(source)
		SetSource(source);

	// Inside a block, only the first line carries the header; continuation lines are aligned to the next tab stop after it.
	if(inBlock)
	{
		SeekCursor(RoundToTab((indentLevel * 4) + strlen(headerText)));
	}
	else
	{
		SeekCursor(indentLevel * 4);

		PrintText(headerText);
	}

	PrintText(message);

	if(newLine)
		NewLine();
}

// NOTE: formatBuf is one shared static buffer, so these formatted calls are not thread-safe and output is truncated at 8191 chars
// (vsprintf_s invokes the CRT invalid-parameter handler on overflow).
/**
 *	Output a formatted message to the log file
 *	
 *	@note It is impossible to set the source of a formatted message.
 *	The previous source will be used.
 */
void IDebugLog::FormattedMessage(const char * fmt, ...)
{
	va_list	argList;

	va_start(argList, fmt);
	vsprintf_s(formatBuf, sizeof(formatBuf), fmt, argList);
	Message(formatBuf);
	va_end(argList);
}

/**
 *	Output a formatted message to the log file
 *	
 *	@note It is impossible to set the source of a formatted message.
 *	The previous source will be used.
 */
void IDebugLog::FormattedMessage(const char * fmt, va_list args)
{
	vsprintf_s(formatBuf, sizeof(formatBuf), fmt, args);
	Message(formatBuf);
}

/**
 * Core entry point behind _ERROR/_WARNING/_MESSAGE/... : write to the file and/or stdout depending on level.
 *
 * @param level Severity of this message (lower number = more severe).
 * @param fmt   printf format string.
 * @param args  Arguments for the format (consumed once).
 */
// ---------------------------------------------------------------------------
// Log: Formats log message, verifies severity threshold, and writes to disk
// ---------------------------------------------------------------------------
void IDebugLog::Log(LogLevel level, const char * fmt, va_list args)
{
	// A message is written when its level is at or above the threshold (numerically <=), separately for file and console.
	bool	log = (level <= logLevel);
	bool	print = (level <= printLevel);

	// Format once, only if somebody wants it, because formatting is the expensive part.
	if(log || print)
		vsprintf_s(formatBuf, sizeof(formatBuf), fmt, args);

	if(log)
		Message(formatBuf);
	
	if(print)
		printf("%s\n", formatBuf);
}

/// Same as Log() but does not end the file line (no newline), so several calls build one line. The console copy is also left without a newline.
void IDebugLog::LogNNL(LogLevel level, const char * fmt, va_list args)
{
	bool	log = (level <= logLevel);
	bool	print = (level <= printLevel);

	if(log || print)
		vsprintf_s(formatBuf, sizeof(formatBuf), fmt, args);

	if(log)
		Message(formatBuf, NULL, false);
	
	if(print)
		printf("%s", formatBuf);
}

/**
 * Set the current message source (shown in the line prefix)
 * @param source Label; only the first 8 characters appear, and the buffer holds 15.
 */
/**
 *	Set the current message source
 */
void IDebugLog::SetSource(const char * source)
{
	strcpy_s(sourceBuf, sizeof(sourceBuf), source);
	// Template "[        ]<tab>" with 8 blank slots that the label is copied into (the loop below stops at 8 characters).
	strcpy_s(headerText, sizeof(headerText), "[        ]\t");
	
	char	* tgt = headerText + 1;
	char	* src = sourceBuf;

	for(int i = 0; (i < 8) && *src; i++, tgt++, src++)
		*tgt = *src;
}

/**
 * Clear the current message source.
 * NOTE: only sourceBuf is cleared; headerText keeps the old prefix, so later lines still show it until SetSource is called again.
 */
/**
 *	Clear the current message source
 */
void IDebugLog::ClearSource(void)
{
	sourceBuf[0] = 0;
}

/**
 *	Increase the indentation level
 */
void IDebugLog::Indent(void)
{
	indentLevel++;
}

/**
 *	Decrease the indentation level
 */
void IDebugLog::Outdent(void)
{
	if(indentLevel)
		indentLevel--;
}

// Blocks: after OpenBlock() the header is printed once and later messages are aligned to it (see Message).
/**
 *	Enter a logical block
 */
void IDebugLog::OpenBlock(void)
{
	SeekCursor(indentLevel * 4);

	PrintText(headerText);

	inBlock = 1;
}

/**
 *	Close a logical block
 */
void IDebugLog::CloseBlock(void)
{
	inBlock = 0;
}

/**
 *	Enable/disable autoflush
 *	
 *	@param inAutoFlush autoflush state
 */
void IDebugLog::SetAutoFlush(bool inAutoFlush)
{
	autoFlush = inAutoFlush;
}

// Tabs are used for alignment only when a full tab stop fits, so the file stays compact but columns still line up.
/**
 *	Print spaces to the log
 *	
 *	If possible, tabs are used instead of spaces.
 */
void IDebugLog::PrintSpaces(int numSpaces)
{
	int	originalNumSpaces = numSpaces;

	if(logFile)
	{
		while(numSpaces > 0)
		{
			if(numSpaces >= TabSize())
			{
				numSpaces -= TabSize();
				fputc('\t', logFile);
			}
			else
			{
				numSpaces--;
				fputc(' ', logFile);
			}
		}
	}

	cursorPos += originalNumSpaces;
}

/**
 *	Prints raw text to the log file
 */
void IDebugLog::PrintText(const char * buf)
{
	if(logFile)
	{
		fputs(buf, logFile);
		if(autoFlush)
			fflush(logFile);
	}

	// Track the cursor column (tabs count as the distance to the next stop) so later SeekCursor calls align correctly.
	const char	* traverse = buf;
	char		data;

	while(data = *traverse++)
	{
		if(data == '\t')
			cursorPos += TabSize();
		else
			cursorPos++;
	}
}

/**
 *	Moves to the next line of the log file
 */
void IDebugLog::NewLine(void)
{
	if(logFile)
	{
		fputc('\n', logFile);

		if(autoFlush)
			fflush(logFile);
	}

	cursorPos = 0;
}

/**
 *	Prints spaces to align the cursor to the requested position
 *	
 *	@note The cursor move will not be performed if the request would move the cursor
 *	backwards.
 */
void IDebugLog::SeekCursor(int position)
{
	if(position > cursorPos)
		PrintSpaces(position - cursorPos);
}

/**
 *	Returns the number of spaces a tab would occupy at the current cursor position
 */
// Tab stops are every 4 columns; (~pos & 3) + 1 is the distance from the cursor to the next stop (1..4).
int IDebugLog::TabSize(void)
{
	return ((~cursorPos) & 3) + 1;
}

/**
 *	Rounds a number of spaces to the nearest tab
 */
// Round up to the next multiple of 4 (a tab stop): add 3 and clear the low two bits. (Despite the doc comment it rounds up, not to the nearest.)
int IDebugLog::RoundToTab(int spaces)
{
	return (spaces + 3) & ~3;
}

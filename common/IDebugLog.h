// ==============================================================================
// Program: IDebugLog.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Diagnostic tracing, multi-level event logging, and indentation framing header.
//          Inputs: Formatted strings, variadic argument lists, and log level filters.
//          Outputs: Synchronized thread-safe output stream to disk or console.
//          Architecture: Foundation logging subsystem for SKSE64 native hook diagnostics.
// ==============================================================================

#pragma once

#include <cstdarg>

/*
 * Process-wide logger writing indented, source-labelled lines to a text file. All state is static, so every IDebugLog
 * object (and the global gLog) writes to the same file; the class is not thread-safe. Callers normally use the
 * _FATALERROR/_ERROR/_WARNING/_MESSAGE/_VMESSAGE/_DMESSAGE helpers defined at the bottom of this header, which map to LogLevel.
 */
/**
 *	A simple debug log file
 *	
 *	This class supports prefix blocks describing the source of the log event.
 *	It also allows logical blocks and outlining.\n
 */
// ---------------------------------------------------------------------------
// IDebugLog: Static logger providing hierarchical indented diagnostic output
// ---------------------------------------------------------------------------
class IDebugLog
{
	public:
							/// Unopened logger.
							IDebugLog();
							/// Logger that opens `name`.
							IDebugLog(const char * name);
							/// Closes the shared file.
							~IDebugLog();

		/// Open (truncate) a log file at an absolute or relative path.
		static void			Open(const char * path);
		/// Open a log inside a Windows known folder (CSIDL id) plus a relative path.
		static void			OpenRelative(int folderID, const char * relPath);

		/// Write one already-formatted message; `source` optionally changes the line label; newLine=false leaves the line open.
		static void			Message(const char * message, const char * source = NULL, bool newLine = true);
		/// printf-style message at the current source.
		static void			FormattedMessage(const char * fmt, ...);
		/// Same with a va_list (for wrappers).
		static void			FormattedMessage(const char * fmt, va_list args);

		// Severity from most to least important; a message is emitted when its level is <= the configured threshold.
		enum LogLevel
		{
			kLevel_FatalError = 0,
			kLevel_Error,
			kLevel_Warning,
			kLevel_Message,
			kLevel_VerboseMessage,
			kLevel_DebugMessage
		};

		/// Log a printf-style message at `level` (file and/or stdout depending on the two thresholds).
		static void			Log(LogLevel level, const char * fmt, va_list args);
		/// Same as Log but without ending the line.
		static void			LogNNL(LogLevel level, const char * fmt, va_list args); // No new line

		/// Set the label shown in the line prefix for subsequent messages.
		static void			SetSource(const char * source);
		/// Clear the label.
		static void			ClearSource(void);

		/// Move the left margin one tab (4 columns) right; nest to show structure.
		static void			Indent(void);
		/// Undo one Indent (never below zero).
		static void			Outdent(void);

		/// Start a multi-line block: the prefix is printed once and later lines are aligned under it.
		static void			OpenBlock(void);
		/// End the block.
		static void			CloseBlock(void);

		/// Flush after every write (true by default) so nothing is lost if the process crashes.
		static void			SetAutoFlush(bool inAutoFlush);

		/// Set the least important level written to the file.
		static void			SetLogLevel(LogLevel in)	{ logLevel = in; }
		/// Set the least important level echoed to the console.
		static void			SetPrintLevel(LogLevel in)	{ printLevel = in; }

	// Internal helpers that track the output column so indentation and alignment come out right.
	private:
		static void			PrintSpaces(int numSpaces);
		static void			PrintText(const char * buf);
		static void			NewLine(void);

		static void			SeekCursor(int position);

		static int			TabSize(void);
		static int			RoundToTab(int spaces);

		static FILE			* logFile;			//!< the output file

		static char			sourceBuf[16];		//!< name of current source, used in prefix
		static char			headerText[16];		//!< current text to use as line prefix
		static char			formatBuf[8192];	//!< temp buffer used for formatted messages

		static int			indentLevel;		//!< the current indentation level (in tabs)
		static int			rightMargin;		//!< the column at which text should be wrapped
		static int			cursorPos;			//!< current cursor position
		static int			inBlock;			//!< are we in a block?

		static bool			autoFlush;			//!< automatically flush the file after writing

		static LogLevel		logLevel;			//!< least important log level to write
		static LogLevel		printLevel;			//!< least important log level to print
};

// The single global logger object; defined in the project that uses this library (this library only declares it).
extern IDebugLog gLog;

// Level-specific helpers. Each captures its variadic arguments and forwards them to gLog.Log() at the matching level.
inline void _FATALERROR(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_FatalError, fmt, args);
	va_end(args);
}

inline void _ERROR(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_Error, fmt, args);
	va_end(args);
}

inline void _WARNING(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_Warning, fmt, args);
	va_end(args);
}

inline void _MESSAGE(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_Message, fmt, args);
	va_end(args);
}

inline void _VMESSAGE(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_VerboseMessage, fmt, args);
	va_end(args);
}

inline void _DMESSAGE(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_DebugMessage, fmt, args);
	va_end(args);
}

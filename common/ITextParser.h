// ==============================================================================
// Program: ITextParser.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Configuration, INI, and plain text line tokenization parser header.
//          Inputs: Raw text data streams, line delimiters, and separator characters.
//          Outputs: Structured key-value pairs, string tokens, and section names.
//          Architecture: Parses skse64.ini configuration and mod settings.
// ==============================================================================


#pragma once

#include "common/IDataStream.h"

/**
 * @class ITextParser
 *
 * Minimal whitespace/line tokenizer over an IDataStream, used when reading plain-text config
 * and data files. It only provides primitives (skip whitespace, read a line, read a token);
 * parsing of INI sections and key=value pairs is done by callers, not here.
 *
 * Usage: Attach() a stream, then loop on HitEOF() calling SkipWhitespace() and
 * ReadToken()/ReadLine(). The parser holds a borrowed pointer and has no thread safety.
 */
// ---------------------------------------------------------------------------
// ITextParser: Line-by-line configuration tokenizer and parser
// ---------------------------------------------------------------------------
class ITextParser
{
public:
	/// Unattached parser; every other method dereferences the stream, so Attach() first.
	ITextParser();
	/// Parser bound to `stream` (borrowed).
	ITextParser(IDataStream * stream);
	~ITextParser();

	/// Rebind the parser to another stream.
	void			Attach(IDataStream * stream);
	/// @return The attached stream (may be NULL).
	IDataStream *	GetStream(void)	{ return m_stream; }

	/// @return true when the underlying stream is at end of data.
	bool	HitEOF(void)	{ return m_stream->HitEOF(); }

	/// Skip spaces, tabs and newlines up to the next visible character.
	void	SkipWhitespace(void);
	/// Skip a run of \n / \r characters only.
	void	SkipLine(void);

	/// Read the rest of the line into `out` (buffer of `length` bytes); the line terminator is dropped.
	void	ReadLine(char * out, UInt32 length);
	/// Read the next whitespace-delimited word into `out` (buffer of `length` bytes), NUL-terminated.
	void	ReadToken(char * out, UInt32 length);

// m_stream: borrowed pointer, never freed by this class.
private:
	IDataStream	* m_stream;
};

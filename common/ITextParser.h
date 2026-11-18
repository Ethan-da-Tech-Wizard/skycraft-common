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

// ---------------------------------------------------------------------------
// ITextParser: Line-by-line configuration tokenizer and parser
// ---------------------------------------------------------------------------
class ITextParser
{
public:
	ITextParser();
	ITextParser(IDataStream * stream);
	~ITextParser();

	void			Attach(IDataStream * stream);
	IDataStream *	GetStream(void)	{ return m_stream; }

	bool	HitEOF(void)	{ return m_stream->HitEOF(); }

	void	SkipWhitespace(void);
	void	SkipLine(void);

	void	ReadLine(char * out, UInt32 length);
	void	ReadToken(char * out, UInt32 length);

private:
	IDataStream	* m_stream;
};

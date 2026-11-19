// ==============================================================================
// Program: ITextParser.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of line splitting, whitespace stripping, and key-value mapping.
//          Inputs: Multi-line text buffers.
//          Outputs: Parsed token lists and section key maps.
//          Algorithm: State machine parser handling comments, quotes, and escape sequences.
// ==============================================================================


#include "ITextParser.h"
#include "IDataStream.h"

// Parses whitespace-separated text from an IDataStream. It never owns the stream.
// ---------------------------------------------------------------------------
// Constructor: Binds text stream and initializes parser cursor
// ---------------------------------------------------------------------------
/// Create a parser with no stream; call Attach() before using it.
ITextParser::ITextParser()
:m_stream(NULL)
{
	//
}

/// @param stream Source of text; borrowed, must outlive the parser.
ITextParser::ITextParser(IDataStream * stream)
:m_stream(stream)
{
	//
}

/// Nothing to release (the stream is not owned).
ITextParser::~ITextParser()
{
	//
}

/// Point the parser at a (new) stream. Position within it is whatever the stream already has.
void ITextParser::Attach(IDataStream * stream)
{
	m_stream = stream;
}

/// Consume leading whitespace (space, tab, newlines) and stop on the first non-space byte or EOF. Uses Peek8 so that byte is not consumed.
void ITextParser::SkipWhitespace(void)
{
	while(!m_stream->HitEOF())
	{
		char	data = m_stream->Peek8();

		if(!isspace(data))
			break;

		m_stream->Skip(1);
	}
}

/// Consume consecutive line-ending characters (\n or \r) only; spaces/tabs are not skipped. Use it after ReadLine to step past the terminator run.
void ITextParser::SkipLine(void)
{
	while(!m_stream->HitEOF())
	{
		char	data = m_stream->Peek8();

		if((data != '\n') && (data != '\r'))
			break;

		m_stream->Skip(1);
	}
}

/**
 * Read one line.
 *
 * @param out    Destination buffer.
 * @param length Buffer size in bytes, including the NUL. The stream's ReadString stops at
 *               either \n or \r, which is not stored.
 */
void ITextParser::ReadLine(char * out, UInt32 length)
{
	m_stream->ReadString(out, length, '\n', '\r');
}

/**
 * Read one whitespace-delimited token, NUL-terminated, truncating to fit.
 *
 * @param buf       Destination buffer.
 * @param bufLength Buffer size in bytes including the terminator; must be > 0 (ASSERT).
 *
 * The delimiter that ends the token is consumed (not left in the stream). If the token is
 * longer than the buffer, the rest stays in the stream for the next call.
 */
void ITextParser::ReadToken(char * buf, UInt32 bufLength)
{
	char	* traverse = buf;

	// Size 1 leaves room only for the terminator: return an empty token without consuming input.
	ASSERT_STR(bufLength > 0, "ITextParser::ReadToken: zero-sized buffer");

	if(bufLength == 1)
	{
		buf[0] = 0;
	}
	else
	{
		// Reserve one byte for the NUL terminator.
		bufLength--;

		for(UInt32 i = 0; (i < bufLength) && !m_stream->HitEOF(); i++)
		{
			UInt8	data = m_stream->Read8();

			// A NUL byte also ends the token so binary junk cannot run on.
			if(isspace(data) || !data)
				break;

			*traverse++ = data;
		}

		*traverse++ = 0;
	}
}

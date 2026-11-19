// ==============================================================================
// Program: IPipeServer.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of named pipe server instance creation and connection loops.
//          Inputs: Pipe configuration flags (PIPE_TYPE_MESSAGE, etc.).
//          Outputs: Client connection acceptance and message pump.
//          Algorithm: Win32 CreateNamedPipe and ConnectNamedPipe synchronization.
// ==============================================================================


#include "IPipeServer.h"

/// Create a server with no pipe open (INVALID_HANDLE_VALUE marks "closed").
IPipeServer::IPipeServer()
:m_pipe(INVALID_HANDLE_VALUE)
{
	//
}

/// Closing here guarantees the Win32 handle is released even if the owner forgot to call Close().
IPipeServer::~IPipeServer()
{
	Close();
}

/**
 * Create the named pipe endpoint (does not wait for a client; see WaitForClient).
 *
 * @param name Full pipe name, e.g. "\\\\.\\pipe\\myname".
 * @return true if the pipe was created; false if creation failed or a pipe with this name
 *         already exists (FILE_FLAG_FIRST_PIPE_INSTANCE makes a second server fail rather than
 *         silently share the name).
 *
 * Any previously open pipe is closed first. Message-type pipe with 8 KiB in/out buffers.
 */
bool IPipeServer::Open(const char * name)
{
	Close();

	// Re-opening an already open server must not leak the old handle.
	m_pipe = CreateNamedPipe(
		name,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
		// Duplex so the loader can both read requests and write replies on the same handle.
		PIPE_TYPE_MESSAGE | PIPE_TYPE_MESSAGE | PIPE_WAIT,
		// NOTE (found while documenting, unchanged here): PIPE_TYPE_MESSAGE is OR-ed twice; the second was probably
		// meant to be PIPE_READMODE_MESSAGE, so reads are not in message mode by default.
		PIPE_UNLIMITED_INSTANCES,
		// Only one client at a time is expected, but the API requires a value; unlimited keeps creation from failing on instance count.
		8192, 8192,
		10 * 1000,	// 10 seconds
		NULL);

	return m_pipe != INVALID_HANDLE_VALUE;
}

/// Close the pipe handle if open. Safe to call repeatedly.
void IPipeServer::Close(void)
{
	if(m_pipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_pipe);
		m_pipe = INVALID_HANDLE_VALUE;
	}
}

/**
 * Block until a client connects.
 *
 * @return true if a client is connected (including one that connected before this call).
 */
bool IPipeServer::WaitForClient(void)
{
	// Blocks because the pipe was created with PIPE_WAIT.
	bool	result = ConnectNamedPipe(m_pipe, NULL) != 0;

	// already connected?
	// ConnectNamedPipe reports ERROR_PIPE_CONNECTED when the client connected between CreateNamedPipe
	// and this call; that is a success, not an error.
	// already connected?
	if(!result)
	{
		if(GetLastError() == ERROR_PIPE_CONNECTED)
			result = true;
	}

	return result;
}

/**
 * Read one request message.
 *
 * @param buf    Buffer for a MessageHeader plus payload.
 * @param length Buffer capacity in bytes.
 * @return true if a whole message (header and the payload its header announces) arrived.
 *
 * NOTE: the ReadFile result is ignored, so on failure bytesRead is uninitialised.
 */
bool IPipeServer::ReadMessage(UInt8 * buf, UInt32 length)
{
	UInt32	bytesRead;

	ReadFile(m_pipe, buf, length, &bytesRead, NULL);

	// The header sits at the start of the buffer and says how long the payload that follows is.
	MessageHeader	* header = (MessageHeader *)buf;

	return
		// has a valid header
		// then the payload length the header claims must also have been received
		(bytesRead >= sizeof(MessageHeader)) &&	// has a valid header
		(bytesRead >= (sizeof(MessageHeader) + header->length));
}

/**
 * Send one message.
 *
 * @param msg Header followed immediately in memory by header->length bytes of payload.
 * @return true if every byte was written.
 */
bool IPipeServer::WriteMessage(MessageHeader * msg)
{
	UInt32	bytesWritten;
	UInt32	length = sizeof(MessageHeader) + msg->length;

	WriteFile(m_pipe, msg, length, &bytesWritten, NULL);

	return bytesWritten >= length;
}

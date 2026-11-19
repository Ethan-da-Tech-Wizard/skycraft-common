// ==============================================================================
// Program: IPipeClient.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of named pipe connection handshakes and synchronous message transfer.
//          Inputs: Pipe address strings and transmission buffers.
//          Outputs: Received message payloads and response codes.
//          Algorithm: Win32 WaitNamedPipe and CreateFile IPC communication.
// ==============================================================================


#include "IPipeClient.h"

/// Create an unconnected client (INVALID_HANDLE_VALUE means "not open").
IPipeClient::IPipeClient()
:m_pipe(INVALID_HANDLE_VALUE)
{
	//
}

/// Closing in the destructor guarantees the handle is released.
IPipeClient::~IPipeClient()
{
	Close();
}

/**
 * Connect to an existing named pipe created by an IPipeServer (client side).
 *
 * @param name Full pipe name, e.g. "\\\\.\\pipe\\myname".
 * @return true if connected; false if the server pipe does not exist yet or is busy.
 *
 * Any previously open connection is closed first. Opened for both read and write, with no
 * sharing, because a message pipe has exactly one client per server instance.
 */
// ---------------------------------------------------------------------------
// Open: Connects to server named pipe using Win32 CreateFile
// ---------------------------------------------------------------------------
bool IPipeClient::Open(const char * name)
{
	// Re-opening must not leak the old handle.
	Close();

	m_pipe = CreateFile(
		name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	return m_pipe != INVALID_HANDLE_VALUE;
}

/// Disconnect from the pipe. Safe to call repeatedly.
void IPipeClient::Close(void)
{
	if(m_pipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_pipe);
		m_pipe = INVALID_HANDLE_VALUE;
	}
}

/**
 * Read one reply message (mirror of IPipeServer::ReadMessage).
 *
 * @param buf    Buffer for a MessageHeader plus payload.
 * @param length Buffer capacity in bytes.
 * @return true if the full header and the payload it announces arrived.
 * NOTE: the ReadFile result is ignored, so on failure bytesRead is uninitialised.
 */
bool IPipeClient::ReadMessage(UInt8 * buf, UInt32 length)
{
	UInt32	bytesRead;

	ReadFile(m_pipe, buf, length, &bytesRead, NULL);

	// The header sits at the start of the buffer and says how long the payload that follows is.
	IPipeServer::MessageHeader	* header = (IPipeServer::MessageHeader *)buf;

	return
		// has a valid header, and then the payload the header claims must be complete too
		(bytesRead >= sizeof(IPipeServer::MessageHeader)) &&	// has a valid header
		(bytesRead >= (sizeof(IPipeServer::MessageHeader) + header->length));
}

/**
 * Send one request message.
 *
 * @param msg Header followed in memory by msg->length bytes of payload.
 * @return true if every byte was written.
 */
bool IPipeClient::WriteMessage(IPipeServer::MessageHeader * msg)
{
	UInt32	bytesWritten;
	UInt32	length = sizeof(IPipeServer::MessageHeader) + msg->length;

	WriteFile(m_pipe, msg, length, &bytesWritten, NULL);

	return bytesWritten >= length;
}

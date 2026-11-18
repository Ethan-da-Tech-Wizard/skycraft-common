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

IPipeClient::IPipeClient()
:m_pipe(INVALID_HANDLE_VALUE)
{
	//
}

IPipeClient::~IPipeClient()
{
	Close();
}

// ---------------------------------------------------------------------------
// Open: Connects to server named pipe using Win32 CreateFile
// ---------------------------------------------------------------------------
bool IPipeClient::Open(const char * name)
{
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

void IPipeClient::Close(void)
{
	if(m_pipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_pipe);
		m_pipe = INVALID_HANDLE_VALUE;
	}
}

bool IPipeClient::ReadMessage(UInt8 * buf, UInt32 length)
{
	UInt32	bytesRead;

	ReadFile(m_pipe, buf, length, &bytesRead, NULL);

	IPipeServer::MessageHeader	* header = (IPipeServer::MessageHeader *)buf;

	return
		(bytesRead >= sizeof(IPipeServer::MessageHeader)) &&	// has a valid header
		(bytesRead >= (sizeof(IPipeServer::MessageHeader) + header->length));
}

bool IPipeClient::WriteMessage(IPipeServer::MessageHeader * msg)
{
	UInt32	bytesWritten;
	UInt32	length = sizeof(IPipeServer::MessageHeader) + msg->length;

	WriteFile(m_pipe, msg, length, &bytesWritten, NULL);

	return bytesWritten >= length;
}

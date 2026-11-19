// ==============================================================================
// Program: IPipeClient.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Win32 Named Pipe client communication interface header.
//          Inputs: Pipe endpoint name (\\.\pipe\skse) and request payloads.
//          Outputs: Bidirectional duplex stream connection to host server process.
//          Architecture: IPC conduit between external managers and SKSE64 runtime.
// ==============================================================================

#pragma once

#include "common/IPipeServer.h"

/**
 * @class IPipeClient
 *
 * Client end of the named-pipe channel served by IPipeServer. Open() connects to an existing pipe, then
 * ReadMessage()/WriteMessage() exchange framed messages (IPipeServer::MessageHeader plus payload).
 */
// ---------------------------------------------------------------------------
// IPipeClient: Named pipe client managing connection lifecycle and stream I/O
// ---------------------------------------------------------------------------
class IPipeClient
{
public:
	IPipeClient();
	virtual ~IPipeClient();

	/// Connect to the server's pipe by name; @return false if it does not exist or is busy.
	bool	Open(const char * name);
	/// Disconnect.
	void	Close(void);
	
	/// Read one framed message into buf; @return true if complete.
	bool	ReadMessage(UInt8 * buf, UInt32 length);
	/// Send one framed message; @return true if fully written.
	bool	WriteMessage(IPipeServer::MessageHeader * msg);

// Win32 pipe handle; INVALID_HANDLE_VALUE when closed.
private:
	HANDLE		m_pipe;
	std::string	m_name;
};

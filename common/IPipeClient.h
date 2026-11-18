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

// ---------------------------------------------------------------------------
// IPipeClient: Named pipe client managing connection lifecycle and stream I/O
// ---------------------------------------------------------------------------
class IPipeClient
{
public:
	IPipeClient();
	virtual ~IPipeClient();

	bool	Open(const char * name);
	void	Close(void);
	
	bool	ReadMessage(UInt8 * buf, UInt32 length);
	bool	WriteMessage(IPipeServer::MessageHeader * msg);

private:
	HANDLE		m_pipe;
	std::string	m_name;
};

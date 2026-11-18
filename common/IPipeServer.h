// ==============================================================================
// Program: IPipeServer.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Win32 Named Pipe server hosting header for IPC listener services.
//          Inputs: Pipe namespace identifier and buffer dimensioning parameters.
//          Outputs: Inbound client connections and dispatched worker threads.
//          Architecture: Hosts telemetry and control pipe inside Skyrim SE process.
// ==============================================================================

#pragma once

// ---------------------------------------------------------------------------
// IPipeServer: Hosts named pipe instances and services client request loops
// ---------------------------------------------------------------------------
class IPipeServer
{
public:
	struct MessageHeader
	{
		UInt32	type;
		UInt32	length;
	};

	IPipeServer();
	virtual ~IPipeServer();

	bool	Open(const char * name);
	void	Close(void);

	bool	WaitForClient(void);

	bool	ReadMessage(UInt8 * buf, UInt32 length);
	bool	WriteMessage(MessageHeader * msg);

private:
	HANDLE	m_pipe;
};

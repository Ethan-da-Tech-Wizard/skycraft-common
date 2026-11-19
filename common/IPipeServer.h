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

/**
 * @class IPipeServer
 *
 * Server end of a Windows named pipe carrying small framed messages (a MessageHeader followed by
 * `length` payload bytes). Open() creates the pipe, WaitForClient() blocks until an IPipeClient connects, then
 * ReadMessage()/WriteMessage() exchange messages. One client at a time; calls block (synchronous pipe).
 */
// ---------------------------------------------------------------------------
// IPipeServer: Hosts named pipe instances and services client request loops
// ---------------------------------------------------------------------------
/// Wire header that precedes every message payload.
class IPipeServer
{
/// Application-defined message type id.
public:
	/// Number of payload bytes that follow this header (the header itself is not counted).
	struct MessageHeader
	{
		UInt32	type;
		UInt32	length;
	};

	IPipeServer();
	virtual ~IPipeServer();

	/// Create the named pipe (fails if the name is already in use).
	bool	Open(const char * name);
	/// Close the pipe handle.
	void	Close(void);

	/// Block until a client connects.
	bool	WaitForClient(void);

	/// Read one framed message into buf; @return true if complete.
	bool	ReadMessage(UInt8 * buf, UInt32 length);
	/// Send one framed message; @return true if fully written.
	bool	WriteMessage(MessageHeader * msg);

private:
	// Win32 pipe handle; INVALID_HANDLE_VALUE when closed.
	HANDLE	m_pipe;
};

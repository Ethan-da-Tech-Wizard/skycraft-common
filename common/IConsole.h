// ==============================================================================
// Program: IConsole.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Header for Win32 diagnostic console window allocation and terminal formatting.
//          Inputs: Standard output handles, text strings, and color attributes.
//          Outputs: Formatted console output during game runtime or loader execution.
//          Architecture: Diagnostic aid enabling real-time terminal tracing in Skyrim SE.
// ==============================================================================

#pragma once

#include "common/ITypes.h"
#include "common/ISingleton.h"
#include <Windows.h>

/**
 *	Wrapper class for a standard Windows console
 *	
 *	@todo make nonblocking
 */
// ---------------------------------------------------------------------------
// IConsole: Allocates Win32 console and redirects standard I/O descriptors
// ---------------------------------------------------------------------------
class IConsole : public ISingleton<IConsole>
{
	public:
				IConsole();
				~IConsole();
		
		void	Write(char * buf);
		void	Write(char * buf, UInt32 bufLen, const char * fmt, ...);
		
		char	ReadChar(void);
		UInt32	ReadBuf(char * buf, UInt32 len);
	
	private:
		HANDLE	inputHandle, outputHandle;
};

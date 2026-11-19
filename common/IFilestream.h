#pragma once

#include "common/IDataStream.h"

/**
 *	An input file stream
 */
// Win32 file implementation of IDataStream (HANDLE based). Open() reads an
// existing file, Create() makes/truncates one; the Browse* variants show the
// standard file dialogs. Close() is also run by the destructor.
class IFileStream : public IDataStream
{
	public:
		IFileStream();
		IFileStream(const char * name);
		virtual ~IFileStream();

		bool	Open(const char * name);
		bool	BrowseOpen(void);

		bool	Create(const char * name);
		bool	BrowseCreate(const char * defaultName = NULL, const char * defaultPath = NULL, const char * title = NULL);

		void	Close(void);

		HANDLE	GetHandle(void)	{ return theFile; }

		virtual void	ReadBuf(void * buf, UInt32 inLength);
		virtual void	WriteBuf(const void * buf, UInt32 inLength);
		virtual void	SetOffset(SInt64 inOffset);

		// can truncate. implicitly seeks to the end of the file
		void	SetLength(UInt64 length);

		// Create every missing directory in `path`.
		static void	MakeAllDirs(const char * path);
		// Returns a pointer to the file-name part inside `path` (no copy is made).
		static char * ExtractFileName(char * path);

	protected:
		HANDLE	theFile;
};

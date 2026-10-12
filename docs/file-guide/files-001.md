# Files 1 of 1 — Skycraft — Common Library Changes

[All pages](../FILE_GUIDE.md) · [Project guide](../PROJECT_GUIDE.md)

| File | Role and useful details |
| --- | --- |
| [.gitignore](../../.gitignore) | Defines local data, credentials, dependencies, and generated outputs that Git should leave untracked. |
| [CMakeLists.txt](../../CMakeLists.txt) | CMake build definition for .; declares targets, dependency discovery, and compiler/build settings. |
| [LICENSE](../../LICENSE) | Preserved license, copyright, or attribution terms; read the full text before reusing the covered component. |
| [PROJECT_NOTES.md](../../PROJECT_NOTES.md) | Preserved detailed setup, usage, architecture, and development notes from the earlier README. |
| [README.md](../../README.md) | Project overview, current status, entry points, setup navigation, and documentation links. |
| [cmake/config.cmake.in](../../cmake/config.cmake.in) | Tracked config.cmake resource in cmake; 100 bytes. |
| [cmake/headerlist.cmake](../../cmake/headerlist.cmake) | CMake build definition for cmake; declares targets, dependency discovery, and compiler/build settings. |
| [cmake/sourcelist.cmake](../../cmake/sourcelist.cmake) | CMake build definition for cmake; declares targets, dependency discovery, and compiler/build settings. |
| [common/.gitignore](../../common/.gitignore) | Defines local data, credentials, dependencies, and generated outputs that Git should leave untracked. |
| [common/IArchive.cpp](../../common/IArchive.cpp) | Source module for IArchive; defines `IArchive::iterator::Instantiate`, `IArchive::iterator::GetBuffer`, `IArchive::iterator::NextOfType`, `IArchive::iterator::PrevOfType`, `IArchive::AttachStream`, `IArchive::Dispose`, `IArchive::ReadHeader`. |
| [common/IArchive.h](../../common/IArchive.h) | Source module for IArchive; defines `implements`, `IArchive`, `iterator`, `FileHeader`, `HeaderEntry`, `begin`, `end` and 8 more symbols. |
| [common/IBufferStream.cpp](../../common/IBufferStream.cpp) | Source module for IBufferStream; defines `IBufferStream::SetBuffer`, `IBufferStream::ReadBuf`, `IBufferStream::WriteBuf`. |
| [common/IBufferStream.h](../../common/IBufferStream.h) | Source module for IBufferStream; defines `IBufferStream`, `GetBuffer`, `OwnBuffer`, `DisownBuffer`. |
| [common/IConsole.cpp](../../common/IConsole.cpp) | Source module for IConsole; defines `IConsole::Write`, `IConsole::ReadChar`, `IConsole::ReadBuf`. |
| [common/IConsole.h](../../common/IConsole.h) | Source module for IConsole; defines `IConsole`. |
| [common/ICriticalSection.h](../../common/ICriticalSection.h) | Source module for ICriticalSection; defines `ICriticalSection`, `IScopedCriticalSection`, `Enter`, `Leave`, `TryEnter`. |
| [common/IDataStream.cpp](../../common/IDataStream.cpp) | Source module for IDataStream; defines `IDataStream::Read8`, `IDataStream::Read16`, `IDataStream::Read32`, `IDataStream::Read64`, `IDataStream::ReadFloat`, `IDataStream::ReadString`, `IDataStream::Peek8` and 25 more symbols. |
| [common/IDataStream.h](../../common/IDataStream.h) | Source module for IDataStream; defines `IDataStream`, `to`, `IDataStream_PositionSaver`, `IDataSubStream`, `Rewind`, `GetParentOffset`, `GetParent` and 1 more symbols. |
| [common/IDatabase.cpp](../../common/IDatabase.cpp) | Source module for IDatabase. |
| [common/IDatabase.h](../../common/IDatabase.h) | Source module for IDatabase; defines `DataType`, `IDatabase`, `Get`, `Alloc`, `Delete`, `GetData`, `Begin` and 2 more symbols. |
| [common/IDatabase.inc](../../common/IDatabase.inc) | Tracked IDatabase resource in common; 1,021 bytes. |
| [common/IDebugLog.cpp](../../common/IDebugLog.cpp) | Source module for IDebugLog; defines `IDebugLog::Open`, `IDebugLog::OpenRelative`, `IDebugLog::Message`, `IDebugLog::FormattedMessage`, `IDebugLog::Log`, `IDebugLog::LogNNL`, `IDebugLog::SetSource` and 12 more symbols. |
| [common/IDebugLog.h](../../common/IDebugLog.h) | Source module for IDebugLog; defines `supports`, `IDebugLog`, `LogLevel`, `SetLogLevel`, `SetPrintLevel`, `_FATALERROR`, `_ERROR` and 4 more symbols. |
| [common/IDirectoryIterator.cpp](../../common/IDirectoryIterator.cpp) | Source module for IDirectoryIterator; defines `IDirectoryIterator::GetFullPath`, `IDirectoryIterator::Next`, `IDirectoryIterator::Done`. |
| [common/IDirectoryIterator.h](../../common/IDirectoryIterator.h) | Source module for IDirectoryIterator; defines `IDirectoryIterator`, `Get`. |
| [common/IDynamicCreate.cpp](../../common/IDynamicCreate.cpp) | Source module for IDynamicCreate; defines `IClassRegistry::RegisterClassInfo`, `IClassRegistry::LookupClassInfo`. |
| [common/IDynamicCreate.h](../../common/IDynamicCreate.h) | Source module for IDynamicCreate; defines `creation`, `__DYN_`, `should`, `IDynamicType`, `allowing`, `declaration`, `definition` and 8 more symbols. |
| [common/IErrors.cpp](../../common/IErrors.cpp) | Source module for IErrors; defines `_AssertionFailed`, `_AssertionFailed_ErrCode`. |
| [common/IErrors.h](../../common/IErrors.h) | Source module for IErrors; defines `StaticAssertFailure`, `static_assert_test`. |
| [common/IEvent.cpp](../../common/IEvent.cpp) | Source module for IEvent; defines `IEvent::Block`, `IEvent::UnBlock`, `IEvent::Wait`. |
| [common/IEvent.h](../../common/IEvent.h) | Source module for IEvent; defines `IEvent`, `IsBlocked`. |
| [common/IFIFO.cpp](../../common/IFIFO.cpp) | Source module for IFIFO; defines `IFIFO::Push`, `IFIFO::Pop`, `IFIFO::Peek`, `IFIFO::Clear`. |
| [common/IFIFO.h](../../common/IFIFO.h) | Source module for IFIFO; defines `IFIFO`, `GetBufferSize`, `GetBufferRemain`, `GetDataLength`, `ToRawOffset`, `ToDataOffset`, `GetWriteOffset`. |
| [common/IFileStream.cpp](../../common/IFileStream.cpp) | Source module for IFileStream; defines `IFileStream::Open`, `BrowseEventProc`, `IFileStream::BrowseOpen`, `IFileStream::Create`, `IFileStream::BrowseCreate`, `IFileStream::Close`, `IFileStream::ReadBuf` and 5 more symbols. |
| [common/IFileStream.h](../../common/IFileStream.h) | Source module for IFileStream; defines `IFileStream`, `GetHandle`. |
| [common/IFilestream.h](../../common/IFilestream.h) | Symbolic-link entry; points to a separate path rather than containing the target’s files. |
| [common/IInterlockedLong.cpp](../../common/IInterlockedLong.cpp) | Source module for IInterlockedLong. |
| [common/IInterlockedLong.h](../../common/IInterlockedLong.h) | Source module for IInterlockedLong; defines `IInterlockedLong`, `Increment`, `Decrement`, `Get`, `Set`, `TrySetIf`, `Claim` and 1 more symbols. |
| [common/ILinkedList.h](../../common/ILinkedList.h) | Source module for ILinkedList; defines `ILink`, `ILinkedList`, `GetObj`, `GetLink`, `Unlink`, `LinkBefore`, `LinkAfter` and 2 more symbols. |
| [common/IMemPool.cpp](../../common/IMemPool.cpp) | Source module for IMemPool; defines `Test_IMemPool`. |
| [common/IMemPool.h](../../common/IMemPool.h) | Source module for IMemPool; defines `IMemPool`, `PoolItem`, `IBasicMemPool`, `IThreadSafeBasicMemPool`, `Reset`, `Allocate`, `Free` and 10 more symbols. |
| [common/IMutex.cpp](../../common/IMutex.cpp) | Source module for IMutex; defines `IMutex::Wait`, `IMutex::Release`. |
| [common/IMutex.h](../../common/IMutex.h) | Source module for IMutex; defines `IMutex`. |
| [common/IPipeClient.cpp](../../common/IPipeClient.cpp) | Source module for IPipeClient; defines `IPipeClient::Open`, `IPipeClient::Close`, `IPipeClient::ReadMessage`, `IPipeClient::WriteMessage`. |
| [common/IPipeClient.h](../../common/IPipeClient.h) | Source module for IPipeClient; defines `IPipeClient`. |
| [common/IPipeServer.cpp](../../common/IPipeServer.cpp) | Source module for IPipeServer; defines `IPipeServer::Open`, `IPipeServer::Close`, `IPipeServer::WaitForClient`, `IPipeServer::ReadMessage`, `IPipeServer::WriteMessage`. |
| [common/IPipeServer.h](../../common/IPipeServer.h) | Source module for IPipeServer; defines `IPipeServer`, `MessageHeader`. |
| [common/IPrefix.cpp](../../common/IPrefix.cpp) | Source module for IPrefix. |
| [common/IPrefix.h](../../common/IPrefix.h) | Source module for IPrefix. |
| [common/IRangeMap.cpp](../../common/IRangeMap.cpp) | Source module for IRangeMap. |
| [common/IRangeMap.h](../../common/IRangeMap.h) | Source module for IRangeMap; defines `IRangeMap`, `Entry`, `Contains`, `Clear`, `Add`, `Lookup`, `Erase` and 4 more symbols. |
| [common/IReadWriteLock.cpp](../../common/IReadWriteLock.cpp) | Source module for IReadWriteLock; defines `IReadWriteLock::StartRead`, `IReadWriteLock::EndRead`, `IReadWriteLock::StartWrite`, `IReadWriteLock::EndWrite`. |
| [common/IReadWriteLock.h](../../common/IReadWriteLock.h) | Source module for IReadWriteLock; defines `IReadWriteLock`. |
| [common/ISegmentStream.cpp](../../common/ISegmentStream.cpp) | Source module for ISegmentStream; defines `ISegmentStream::AttachStream`, `ISegmentStream::AddSegment`, `ISegmentStream::ReadBuf`, `ISegmentStream::WriteBuf`, `ISegmentStream::SetOffset`, `ISegmentStream::LookupInfo`. |
| [common/ISegmentStream.h](../../common/ISegmentStream.h) | Source module for ISegmentStream; defines `ISegmentStream`, `SegmentInfo`. |
| [common/ISingleton.cpp](../../common/ISingleton.cpp) | Source module for ISingleton. |
| [common/ISingleton.h](../../common/ISingleton.h) | Source module for ISingleton; defines `that`, `ISingleton`, `GetSingleton`, `GetSingletonPtr`. |
| [common/ITextParser.cpp](../../common/ITextParser.cpp) | Source module for ITextParser; defines `ITextParser::Attach`, `ITextParser::SkipWhitespace`, `ITextParser::SkipLine`, `ITextParser::ReadLine`, `ITextParser::ReadToken`. |
| [common/ITextParser.h](../../common/ITextParser.h) | Source module for ITextParser; defines `ITextParser`, `GetStream`, `HitEOF`. |
| [common/IThread.cpp](../../common/IThread.cpp) | Source module for IThread; defines `IThread::Start`, `IThread::Stop`, `IThread::ForceStop`, `IThread::_ThreadProc`. |
| [common/IThread.h](../../common/IThread.h) | Source module for IThread; defines `IThread`, `IsRunning`, `StopRequested`, `GetHandle`. |
| [common/ITimer.cpp](../../common/ITimer.cpp) | Source module for ITimer; defines `ITimer::Init`, `ITimer::DeInit`, `ITimer::Start`, `ITimer::GetElapsedTime`, `ITimer::GetQPC`. |
| [common/ITimer.h](../../common/ITimer.h) | Source module for ITimer; defines `ITimer`. |
| [common/ITypes.cpp](../../common/ITypes.cpp) | Source module for ITypes; defines `Bitstring::Alloc`, `Bitstring::Dispose`, `Bitstring::Clear`, `Bitstring::Set`, `Bitstring::IsSet`, `Bitstring::IsClear`. |
| [common/ITypes.h](../../common/ITypes.h) | Source module for ITypes; defines `Bitfield`, `Bitstring`, `Time`, `void`, `to`, `has`, `Vector2` and 37 more symbols. |
| [common/common.vcproj](../../common/common.vcproj) | Tracked common resource in common; 6,956 bytes. |
| [common/common.vcxproj](../../common/common.vcxproj) | Configuration or build metadata for common; read alongside the application or tool that loads it. |
| [common/common.vcxproj.filters](../../common/common.vcxproj.filters) | Tracked common.vcxproj resource in common; 6,020 bytes. |
| [common/common_vc11.sln](../../common/common_vc11.sln) | Tracked common vc11 resource in common; 844 bytes. |
| [common/common_vc11.vcxproj](../../common/common_vc11.vcxproj) | Configuration or build metadata for common vc11; read alongside the application or tool that loads it. |
| [common/common_vc11.vcxproj.filters](../../common/common_vc11.vcxproj.filters) | Tracked common vc11.vcxproj resource in common; 3,338 bytes. |
| [common/common_vc14.sln](../../common/common_vc14.sln) | Tracked common vc14 resource in common; 919 bytes. |
| [common/common_vc14.vcxproj](../../common/common_vc14.vcxproj) | Configuration or build metadata for common vc14; read alongside the application or tool that loads it. |
| [common/common_vc14.vcxproj.filters](../../common/common_vc14.vcxproj.filters) | Tracked common vc14.vcxproj resource in common; 3,245 bytes. |
| [common/common_vc9.sln](../../common/common_vc9.sln) | Tracked common vc9 resource in common; 865 bytes. |
| [common/common_vc9.vcproj](../../common/common_vc9.vcproj) | Tracked common vc9 resource in common; 7,009 bytes. |
| [docs/DEPENDENCIES.csv](../../docs/DEPENDENCIES.csv) | Machine-readable inventory of declared and locked dependencies, versions, reported licenses, and evidence URLs. |
| [docs/FILE_GUIDE.md](../../docs/FILE_GUIDE.md) | Index of the complete tracked-file guide, with one entry for every file and links to the paginated descriptions. |
| [docs/LICENSE_REVIEW.md](../../docs/LICENSE_REVIEW.md) | Original-code license, preserved component notices, dependency-license evidence, and unresolved distribution questions. |
| [docs/PROJECT_GUIDE.md](../../docs/PROJECT_GUIDE.md) | Maintainer orientation: purpose, important entry points, source layout, commands, and project-specific boundaries. |
| [docs/file-guide/files-001.md](../../docs/file-guide/files-001.md) | One page of the tracked-file catalog; links each listed path to its role and source-derived details. |
| [generate_cmake.py](../../generate_cmake.py) | Python implementation of generate cmake. Defines `main`. |

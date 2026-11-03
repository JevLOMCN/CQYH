/*=============================================================================
	FFileManagerMPQ.h: Unreal Windows OS based file manager.
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "CorePrivate.h"
#include "UnFile.h"

#if WITH_STORMLIB

#include "StormPort.h"
#include "StormLib.h"

#if _WINDOWS

#include "FFileManagerMPQ.h"
#include "FFileManagerWindows.h"

#pragma pack (push,8)
#include <sys/stat.h>
#include <sys/utime.h>
#include <time.h>
#include <shlobj.h>
#include <map>
#pragma pack(pop)

//------------------------------------------------------------------------------------------------
// 文件名HASH映射
//------------------------------------------------------------------------------------------------
#if M_HASH_FILE_NAME

extern const TCHAR* g_fileHashTable[];
extern INT g_fileHashTableSize;

struct FMapStringCompare
{
	bool operator()(const TCHAR* lhs, const TCHAR* rhs)
	{
		return appStricmp(lhs, rhs) < 0;
	}
};

typedef std::map<const TCHAR*, const TCHAR*, FMapStringCompare> FileNameHashMap;
FileNameHashMap GFileHashMap;

bool InitFileNameHashTable()
{
	TArray<FString> dbgLog;
	for (INT i = 0; i < g_fileHashTableSize; i += 2)
	{
#if 1
		FileNameHashMap::const_iterator it = GFileHashMap.find(g_fileHashTable[i + 1]);
		if(it != GFileHashMap.end())
		{
			appOutputDebugStringf(TEXT("Hash file name:%s : %s -> %s\n"), it->first, it->second, g_fileHashTable[i + 1]); 
		}

		appOutputDebugStringf(TEXT("Hash[%d]:%s -> %s\n"), i, g_fileHashTable[i + 1], g_fileHashTable[i]); 
#endif

		GFileHashMap.insert(std::make_pair(g_fileHashTable[i + 1], g_fileHashTable[i]));
	}

#ifdef _DEBUG
	check(GFileHashMap.size() == g_fileHashTableSize / 2);
#endif
	return true;
}

static bool g_InitFileNameHashTable = InitFileNameHashTable();


FString TryFindHashFileName( const TCHAR* FilenameSrc )
{
	FFilename fileName(FilenameSrc);
	FileNameHashMap::const_iterator it = GFileHashMap.find(*fileName.GetCleanFilename());
	if(it != GFileHashMap.end())
	{
		if(fileName.GetPath().Len() > 0)
		{
			return fileName.GetPath() + TEXT("\\") + it->second;
		}
		return it->second;
	}
	else
	{		
		//appOutputDebugStringf(TEXT("Failed find hash for:%s\n"), FilenameSrc);
		return FilenameSrc;
	}
}


#endif

// Encode manager Add by JimZhang@snda.com
//加密算法
//FArchiveEncode::FArchiveEncode()
//{
//	UINT	 seedEncode = 897502;
//	for(int i=0;i<256;i++)
//	{
//		offsetEnCode[i] = i;
//	}
//	//乱序
//	srand(seedEncode);
//	BYTE buf;
//	int  iBuf;
//	for(int i=0;i<256;i++)
//	{
//		iBuf = rand()%256; 
//		buf  = offsetEnCode[i];
//		offsetEnCode[i]		= offsetEnCode[iBuf];
//		offsetEnCode[iBuf]	= buf;
//	}
//	//解乱序
//	for(int i=0;i<256;i++)
//	{
//		offsetDeCode[offsetEnCode[i]] = i;
//	}
//}
//
//FArchiveEncode::~FArchiveEncode()
//{
//
//}

/*-----------------------------------------------------------------------------
	FArchiveFileReaderMPQ implementation
-----------------------------------------------------------------------------*/

FArchiveFileReaderMPQ::FArchiveFileReaderMPQ( HANDLE InHandle, INT InStatsHandle, const TCHAR* InFilename, FOutputDevice* InError, INT InSize )
:   Handle          ( InHandle )
,	StatsHandle		( InStatsHandle )
,	Filename		( InFilename )
,   Error           ( InError )
,   Size            ( InSize )
,   Pos             ( 0 )
,   BufferBase      ( 0 )
,   BufferCount     ( 0 )
{
	ArIsLoading = ArIsPersistent = 1;
}

FArchiveFileReaderMPQ::~FArchiveFileReaderMPQ()
{
	FILE_IO_STATS_CLOSE_HANDLE( StatsHandle );
	if( Handle )
	{
		Close();
	}
}

UBOOL FArchiveFileReaderMPQ::InternalPrecache( INT PrecacheOffset, INT PrecacheSize )
{
	// Only precache at current position and avoid work if precaching same offset twice.
	if( Pos == PrecacheOffset && (!BufferBase || !BufferCount || BufferBase != Pos) )
	{
		BufferBase = Pos;
		BufferCount = Min( Min( PrecacheSize, (INT)(ARRAY_COUNT(Buffer) - (Pos&(ARRAY_COUNT(Buffer)-1))) ), Size-Pos );
		BufferCount = Max(BufferCount,0); // clamp to 0
		DWORD Count=0;

		// Read data from device via stormlib ReadFile API.
		{
			SCOPED_FILE_IO_READ_STATS( StatsHandle, BufferCount, Pos );
			SFileReadFile(Handle, Buffer, BufferCount, &Count, NULL);
			FArchiveEncode::DecodeAlways(Buffer, BufferCount); // oscar@snda
		}

		if( Count!=BufferCount )
		{
			TCHAR ErrorBuffer[1024];
			ArIsError = 1;
			Error->Logf( TEXT("ReadFile failed: Count=%i BufferCount=%i Error=%s"), Count, BufferCount, appGetSystemErrorMessage(ErrorBuffer,1024) );
		}
	}
	return TRUE;
}

void FArchiveFileReaderMPQ::Seek( INT InPos )
{
	SCOPE_CYCLE_COUNTER(STAT_LinkerLoad_Seek);

	check(InPos>=0);
	check(InPos<=Size);
	if( SFileSetFilePointer( Handle, InPos, NULL, FILE_BEGIN )==INVALID_SET_FILE_POINTER )
	{
		TCHAR ErrorBuffer[1024];
		ArIsError = 1;
		Error->Logf( TEXT("SetFilePointer Failed %i/%i: %i %s"), InPos, Size, Pos, appGetSystemErrorMessage(ErrorBuffer,1024) );
	}
	Pos         = InPos;
	BufferBase  = Pos;
	BufferCount = 0;
}

INT FArchiveFileReaderMPQ::Tell()
{
	return Pos;
}

INT FArchiveFileReaderMPQ::TotalSize()
{
	return Size;
}

UBOOL FArchiveFileReaderMPQ::Close()
{
	if( Handle )
	{
		BOOL hr = SFileCloseFile( Handle ); // oscar@snda
		dbgCloseFile(*Filename, Handle, hr); // oscar@snda
		TRACE_FILE_HANDLE_REMOVE(Handle); // oscar@snda
	}

	Handle = NULL;
	return !ArIsError;
}

void FArchiveFileReaderMPQ::Serialize( void* V, INT Length )
{
	while( Length>0 )
	{
		INT Copy = Min( Length, BufferBase+BufferCount-Pos );
		if( Copy<=0 )
		{
			if( Length >= ARRAY_COUNT(Buffer) )
			{
				INT Count=0;
				OVERLAPPED o;
				// Read data from device via stormlib ReadFile API.
				{
					SCOPED_FILE_IO_READ_STATS( StatsHandle, Length, Pos );

					SFileReadFile(Handle, V, Length, (DWORD*)&Count, &o);
					
					FArchiveEncode::DecodeAlways((BYTE*)V, Length); // oscar@snda
				}
				if( Count!=Length )
				{
					TCHAR ErrorBuffer[1024];
					ArIsError = 1;
					Error->Logf( TEXT("ReadFile failed: Count=%i Length=%i Error=%s for file %s"), 
						Count, Length, appGetSystemErrorMessage(ErrorBuffer,1024), *Filename );
				}
				Pos += Length;
				BufferBase += Length;
				return;
			}
			InternalPrecache( Pos, MAXINT );
			Copy = Min( Length, BufferBase+BufferCount-Pos );
			if( Copy<=0 )
			{
				ArIsError = 1;
				Error->Logf( TEXT("ReadFile beyond EOF %i+%i/%i for file %s"), 
					Pos, Length, Size, *Filename );
			}
			if( ArIsError )
			{
				return;
			}
		}
		appMemcpy( V, Buffer+Pos-BufferBase, Copy );
		Pos       += Copy;
		Length    -= Copy;

		//FArchiveEncode::Decode((BYTE*)V, Copy); // oscar@snda

		// leave this at end
		V          = (BYTE*)V + Copy;
	}
}



/*-----------------------------------------------------------------------------
	FArchiveFileWriterMPQ implementation
-----------------------------------------------------------------------------*/

FArchiveFileWriterMPQ::FArchiveFileWriterMPQ( HANDLE InHandle, INT InStatsHandle, const TCHAR* InFilename, FOutputDevice* InError, INT InPos )
:   Handle      ( InHandle )
,	StatsHandle ( InStatsHandle )
,	Filename	( InFilename )
,   Error       ( InError )
,   Pos         ( InPos )
,   BufferCount ( 0 )
{
	ArIsSaving = ArIsPersistent = 1;
}

FArchiveFileWriterMPQ::~FArchiveFileWriterMPQ()
{
	if( Handle )
	{
		Close();
	}
	Handle = NULL;
}

void FArchiveFileWriterMPQ::Seek( INT InPos )
{
	Flush();
	if( SFileSetFilePointer( Handle, InPos, NULL, FILE_BEGIN )==SFILE_INVALID_SIZE )
	{
		ArIsError = 1;
		Error->Logf( *LocalizeError("SeekFailed",TEXT("Core")) );
	}
	Pos = InPos;
}
	
INT FArchiveFileWriterMPQ::Tell()
{
	return Pos;
}

INT FArchiveFileWriterMPQ::TotalSize()
{
	// Make sure that all data is written before looking at file size.
	Flush();

	// Determine size of file.
	DWORD dwLow = 0, dwHigh = 0;
	dwLow = SFileGetFileSize( Handle, &dwHigh );

	if(dwLow == 0 && dwHigh == 0)
	{
		ArIsError = 1;
		Error->Logf( *LocalizeError("WriteFailed",TEXT("Core")) );
	}
	return dwLow;
}

UBOOL FArchiveFileWriterMPQ::Close()
{
	Flush();
	if( Handle && !SFileCloseFile(Handle) )
	{
		ArIsError = 1;
		Error->Logf( *LocalizeError("WriteFailed",TEXT("Core")) );
	}
	Handle = NULL;
	return !ArIsError;
}

void FArchiveFileWriterMPQ::Serialize( void* V, INT Length )
{
	Pos += Length;
	INT Copy;
	while( Length > (Copy=ARRAY_COUNT(Buffer)-BufferCount) )
	{
		appMemcpy( Buffer+BufferCount, V, Copy );
		////加密运算
		//DoEncode(Buffer+BufferCount,Copy);
		BufferCount += Copy;
		Length      -= Copy;
		V            = (BYTE*)V + Copy;
		Flush();
	}
	if( Length )
	{
		appMemcpy( Buffer+BufferCount, V, Length );
		////加密运算
		//DoEncode(Buffer+BufferCount,Length);
		BufferCount += Length;
	}



	//if(Filename.Right(7) == "GFxUI.u"||Filename.Right(8) == "GFxUI.u")
	//{
	//	warnf( NAME_Warning,TEXT("--------------------------FArchiveFileWriterMPQ::Flush(): %s = %d"),  *Filename,BufferCount);
	//}
	////::MessageBox(NULL,*Filename,TEXT(""),NULL);
}

void FArchiveFileWriterMPQ::Flush()
{
	if( BufferCount )
	{
		SCOPED_FILE_IO_WRITE_STATS( StatsHandle, BufferCount, Pos );

		INT Result=0;

		FArchiveEncode::Encode(Buffer, BufferCount); // oscar@snda
		
		//if( !WriteFile( Handle, Buffer, BufferCount, (DWORD*)&Result, NULL ) )
		if(!SFileWriteFile(Handle, Buffer, BufferCount, PAK_COMPRESS_TYPE))
		{
			ArIsError = 1;
			Error->Logf( *LocalizeError("WriteFailed",TEXT("Core")) );
		}
	}
	BufferCount = 0;
}


/*-----------------------------------------------------------------------------
	FFileManagerMPQ implementation
-----------------------------------------------------------------------------*/

/**
 * Converts passed in filename to use a relative path.
 *
 * @param	Filename	filename to convert to use a relative path
 * 
 * @return	filename using relative path
 */
FString FFileManagerMPQ::ConvertToRelativePath( const TCHAR* Filename )
{
	//default to the full absolute path of this file
	FString RelativePath = Filename;

	// See whether it is a relative path.
	FString RootDir = appRootDir();
	//the default relative directory it to the app root which is 2 directories up from the starting directory
	//Drive:\Build\UnrealEngine3\ from Drive:\Build\UnrealEngine3\Binaries\Win32
	INT NumberOfDirectoriesToGoUp = 2;

	//temp holder for current position of the slash
	INT CurrentSlashPosition;

	//while we haven't run out of parent directories until we which a drive name
	const UBOOL bStartFromEnd = TRUE;
	const UBOOL bIgnoreCase = TRUE;
	while ( (CurrentSlashPosition = RootDir.InStr(TEXT("\\"), bStartFromEnd)) != INDEX_NONE)
	{
		if( RelativePath.StartsWith( RootDir ) )
		{
			// And if that's the case append the base dir.
			RelativePath = RelativePath.Right(RelativePath.Len() - RootDir.Len());
			for (INT ParentDirectoryLoopIndex = 0; ParentDirectoryLoopIndex < NumberOfDirectoriesToGoUp; ParentDirectoryLoopIndex++)
			{
				RelativePath = 	FString::Printf( TEXT("..") PATH_SEPARATOR TEXT("%s"), *RelativePath);
			}
			break;
		}
		INT PositionOfNextSlash = RootDir.InStr(TEXT("\\"), bStartFromEnd, bIgnoreCase, CurrentSlashPosition);
		//if there is another slash to find
		if (PositionOfNextSlash != INDEX_NONE)
		{
			//move up a directory and on an extra .. PATH_SEPARATOR
			// the +1 from "InStr" moves to include the "\" at the end of the directory name
			NumberOfDirectoriesToGoUp++;
			RootDir = RootDir.Left( PositionOfNextSlash + 1 );
		}
		else
		{
			RootDir.Empty();
		}
	}

	
	return RelativePath;
}

/**
 * Converts passed in filename to use an absolute path.
 *
 * @param	Filename	filename to convert to use an absolute path, safe to pass in already using absolute path
 * 
 * @return	filename using absolute path
 */
FString FFileManagerMPQ::ConvertToAbsolutePath( const TCHAR* Filename )
{
	FString AbsolutePath = Filename;

	// See whether it is a relative path.
	if( AbsolutePath.StartsWith( TEXT("..") ) )
	{
		// And if that's the case append the base dir.
		AbsolutePath = FString(appBaseDir()) + AbsolutePath;
	}

	return AbsolutePath;
}

/**
 * Converts a path pointing into the installed directory (C:\Program Files\MyGame\ExampleGame\Config\ExampleEngine.ini)
 * to a path that a least-privileged user can write to (C:\<UserDir>\MyGame\ExampleGame\Config\ExampleEngine.ini)
 *
 * @param AbsolutePath Source path to convert
 *
 * @return Path to the user directory
 */
FString FFileManagerMPQ::ConvertAbsolutePathToUserPath(const TCHAR* AbsolutePath)
{
	// if the user directory has been setup, use it
	if (WindowsUserDir.Len() > 0)
	{
		const UBOOL bUseUnpublished = GIsEditor
			|| (appStristr(appCmdLine(), TEXT("Editor")) != NULL)
			|| (appStristr(appCmdLine(), TEXT("CookPackages")) != NULL)
			|| (appStristr(appCmdLine(), TEXT("UseUnpublished")) != NULL);

		const TCHAR* ReplaceBit;
		const TCHAR* UnpublishedPath;
		const TCHAR* PublishedPath;

		// @todo ship: Use GSys->Paths or something for the replacement, instead of hardcoded Content
		FString UserContentPath(AbsolutePath);
		for (INT Path = 0; Path < 2; Path++)
		{
			if (Path == 0)
			{
				ReplaceBit = TEXT("\\CookedPC\\");
				UnpublishedPath = TEXT("\\Unpublished\\CookedPC\\");
				PublishedPath = TEXT("\\Published\\CookedPC\\");
			}
			else
			{
				ReplaceBit = TEXT("\\Content\\");
				UnpublishedPath = TEXT("\\Unpublished\\Content\\");
				PublishedPath = TEXT("\\Published\\Content\\");
			}

			// make sure we aren't using one of the special paths
			if (UserContentPath.InStr(UnpublishedPath, FALSE, TRUE) == INDEX_NONE && UserContentPath.InStr(PublishedPath, FALSE, TRUE) == INDEX_NONE)
			{
				UserContentPath = UserContentPath.Replace(ReplaceBit, bUseUnpublished ? UnpublishedPath : PublishedPath);
			}
		}

		// replace C:\Program Files\MyGame\Binaries with C:\<UserDir>\Binaries
		return UserContentPath.Replace(*WindowsRootDir, *WindowsUserDir, TRUE);
	}
	else
	{
		// in the non-installed (dev) case, just return the original path in all cases
		return AbsolutePath;
	}
}


/*-----------------------------------------------------------------------------
	Public interface
-----------------------------------------------------------------------------*/


FFileManagerMPQ::~FFileManagerMPQ()
{
	for (TMap<FString, HANDLE>::TIterator itor(MapMPQHandle); itor; ++itor)
	{
		const FString& rKey = itor.Key();
		HANDLE& h = itor.Value();

		if (h != NULL)
		{
			SFileCloseArchive(h);
		}
	}

	MapMPQHandle.Empty();
}

void FFileManagerMPQ::Init(UBOOL Startup)
{
	// a shipped PC game will always run as if installed
#if SHIPPING_PC_GAME && !UDK && !DEDICATED_SERVER
	// shipping PC game 
	bIsRunningInstalled = TRUE;
#else
	// for development, use a commandline param (-installed)
	bIsRunningInstalled = ParseParam(appCmdLine(),TEXT("installed"));
#endif

	//////////////////////////////////////////////////////////////////////////
	if (Startup)
	{
		// zhaojun@snda add this section
		TCHAR ContentDir[1024];

		swprintf_s(ContentDir, sizeof(ContentDir) / sizeof(ContentDir[0]), 
			TEXT("..%s..%s%sGame"), PATH_SEPARATOR, PATH_SEPARATOR, appGetGameName());

		FString FullPath = RELATIVE_TO_FULL_FUNC(ContentDir);

		GenerateMapFromFileToMPQ(*FullPath);
	}

	// Allow overriding use of My Documents folder with -NOHOMEDIR
	if( ParseParam(appCmdLine(),TEXT("NOHOMEDIR") ) )
	{
		bIsRunningInstalled = FALSE;
	}

	if (bIsRunningInstalled)
	{
		debugf( TEXT( " ... running in INSTALLED mode" ) );

		TCHAR UserPath[MAX_PATH];
		// get the My Documents directory
		HRESULT Ret = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, UserPath);

		// get the per-game directory name to use inside the My Documents directory 
		FString DefaultIniContents;
		// load the DefaultEngine.ini config file into a string for later parsing (ConvertAbsolutePathToUserPath will use
		// original location since WindowsUserDir hasn't been set yet)
		// can't use GDefaultEngineIni, because that may be something that doesn't have the tag
		if (!appLoadFileToString(DefaultIniContents, *(appGameConfigDir() + TEXT("DefaultEngine.ini")), this))
		{
			// appMsgf won't write to a log if GWarn is NULL, which it should be at this point
			appMsgf(AMT_OK, TEXT("Failed to find default engine .ini file to retrieve My Documents subdirectory to use. Force quitting."));
			exit(1);
			return;
		}

		#define MYDOC_KEY_NAME TEXT("MyDocumentsSubDirName=")

		// find special key in the .ini file (can't use GConfig because it can't be used yet until after filemanager is made)
		INT KeyLocation = DefaultIniContents.InStr(MYDOC_KEY_NAME, FALSE, TRUE);
		if (KeyLocation == INDEX_NONE)
		{
			// appMsgf won't write to a log if GWarn is NULL, which it should be at this point
			appMsgf(AMT_OK, TEXT("Failed to find %s key in DefaultEngine.ini. Force quitting."), MYDOC_KEY_NAME);
			exit(1);
			return;
		}

		// skip over the key to get the value (skip key and = sign) and everything after it
		FString ValueAndLeftover = DefaultIniContents.Mid(KeyLocation + appStrlen(MYDOC_KEY_NAME));
		
		// now chop off this string at an end of line
		TArray<FString> Tokens;
		ValueAndLeftover.ParseIntoArray(&Tokens, TEXT("\r\n"), TRUE);

		// make the base user dir path
		WindowsUserDir = FString(UserPath) 
							+ TEXT("\\My Games\\") 
							+ Tokens(0) 
#if DEMOVERSION
							+ TEXT(" Demo")
#endif
							+ TEXT("\\");

		// oscar@snda
		FString OldWindowsUserDir = FString(UserPath) + TEXT("\\My Games\\") + TEXT("Mir3D")
#if DEMOVERSION
			+ TEXT(" Demo")
#endif
			+ TEXT("\\");

		// 如果有旧的目录并且没有新的目录，则把旧的目录重命名过来，只做一次即可
		if(appDirectoryExists(*OldWindowsUserDir) && !appDirectoryExists(*WindowsUserDir))
		{
			BOOL moveResult = ::MoveFileW(*OldWindowsUserDir, *WindowsUserDir);
			OutputDebugStringW(*FString::Printf(TEXT("Move dir result:%d from\n %s \n to \n %s"), (INT)moveResult, *OldWindowsUserDir, *WindowsUserDir));
		}
		// oscar@snda

		// find out our executable path
		WindowsRootDir = appBaseDir();
		// strip off the Binaries directory
		WindowsRootDir = WindowsRootDir.Left(WindowsRootDir.InStr(TEXT("\\Binaries\\"), TRUE, TRUE) + 1);

		// Now that the root directory has been set, create directories at startup.
		// Note this must come after the above because MakeDirectory calls
		// ConvertAbsolutePathToUserPath which uses WindowsRootDir and WindowsUserDir.
		#define DIRSTOCREATATSTARTUP_KEY_NAME TEXT("DirsToCreateAtStartup=")
		INT FindStartPos = INDEX_NONE;
		while ( TRUE )
		{
			// find special key in the .ini file (can't use GConfig because it can't be used yet until after filemanager is made)
			const INT KeyLocation = DefaultIniContents.InStr(DIRSTOCREATATSTARTUP_KEY_NAME, FALSE, TRUE, FindStartPos);
			if (KeyLocation == INDEX_NONE)
			{
				break;
			}
			// Advance the find pos because we're doing a multi find.
			FindStartPos = KeyLocation + appStrlen(DIRSTOCREATATSTARTUP_KEY_NAME);

			// skip over the key to get the value (skip key and = sign) and everything after it
			FString ValueAndLeftover = DefaultIniContents.Mid(KeyLocation + appStrlen(DIRSTOCREATATSTARTUP_KEY_NAME));
			
			// now chop off this string at an end of line
			TArray<FString> Tokens;
			ValueAndLeftover.ParseIntoArray(&Tokens, TEXT("\r\n"), TRUE);

			// Create the directory.
			MakeDirectory( *Tokens(0), TRUE );
		}
	}

	FFileManagerGeneric::Init(Startup);
}

UBOOL FFileManagerMPQ::SetDefaultDirectory()
{
	return SetCurrentDirectoryW(appBaseDir())!=0;
}

UBOOL FFileManagerMPQ::SetCurDirectory(const TCHAR* Directory)
{
	return SetCurrentDirectoryW(Directory)!=0;
}

FString FFileManagerMPQ::GetCurrentDirectory()
{
	return appBaseDir();
}

FArchive* FFileManagerMPQ::CreateFileReader( const TCHAR* InFilename, DWORD Flags, FOutputDevice* Error )
{
	SCOPE_CYCLE_COUNTER(STAT_CreateFileReaderMPQ);

	// first look in User Directory
	FArchive* ReturnValue = InternalCreateFileReader( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(InFilename)), Flags, Error );

	// if not found there, then look in the install directory
	if (ReturnValue == NULL)
	{
		ReturnValue = InternalCreateFileReader( *ConvertToAbsolutePath(InFilename), Flags, Error );
	}

	return ReturnValue;
}

/**
* Dummy archive that doesn't actually write anything
* it just updates the file pos when seeking
*/
class FArchiveFileWriterDummyMPQ : public FArchive
{
public:
	FArchiveFileWriterDummyMPQ()
		:	Pos(0)
	{
		ArIsSaving = ArIsPersistent = 1;
	}
	virtual ~FArchiveFileWriterDummyMPQ()
	{
		Close();
	}
	virtual void Seek( INT InPos )
	{
		Pos = InPos;
	}
	virtual INT Tell()
	{
		return Pos;
	}
	virtual void Serialize( void* V, INT Length )
	{
		Pos += Length;
	}
	virtual FString GetArchiveName() const { return TEXT("FArchiveFileWriterDummyMPQ"); }
protected:
	INT             Pos;
};

FArchive* FFileManagerMPQ::CreateFileWriter( const TCHAR* Filename, DWORD Flags, FOutputDevice* Error, INT MaxFileSize )
{
	// Only allow writes to files that are not signed 
	// Except if the file is missing (that way corrupt ini files can be autogenerated by deleting them)
	if( FSHA1::GetFileSHAHash(Filename, NULL) &&
		FileSize(Filename) != -1 )
	{
		debugfSuppressed(NAME_DevSHA,TEXT("Can't write to signed game file: %s"),Filename);
		return new FArchiveFileWriterDummyMPQ();
	}
	//::MessageBox(NULL,Filename,TEXT(""),NULL);
	return InternalCreateFileWriter( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)), Flags, Error );
}

/**
 *	Returns the size of a file. (Thread-safe)
 *
 *	@param Filename		Platform-independent Unreal filename.
 *	@return				File size in bytes or -1 if the file didn't exist.
 **/
INT FFileManagerMPQ::FileSize( const TCHAR* Filename )
{
	// try user directory first
	INT ReturnValue = InternalFileSize( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)) );
	if (ReturnValue == -1)
	{
		ReturnValue = InternalFileSize( *ConvertToAbsolutePath(Filename) );
	}
	return ReturnValue;
}

INT FFileManagerMPQ::UncompressedFileSize( const TCHAR* Filename )
{
	// default to not using the uncompressed file size
	INT Result = -1;	
	// check for the presence of a .uncompressed_size manifest file which mirrors the Filename
	// if it exists, then the file was fully compressed, so we get the original uncompressed size for it
	FString UnCompressedSizeFilename = FString(Filename) + FString(TEXT(".uncompressed_size"));
	if( FileSize(*UnCompressedSizeFilename) != -1 )
	{
		FString SizeString;
		appLoadFileToString(SizeString, *UnCompressedSizeFilename);
		check(SizeString.Len());
		// get the uncompressed size from the file
		Result = appAtoi(*SizeString);
	}
	return Result;
}

DWORD FFileManagerMPQ::Copy( const TCHAR* DestFile, const TCHAR* SrcFile, UBOOL ReplaceExisting, UBOOL EvenIfReadOnly, UBOOL Attributes, FCopyProgress* Progress )
{
	// we can only write to user directory, but source may be user or install (try user first)
	DWORD ReturnValue = InternalCopy( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(DestFile)), *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(SrcFile)), ReplaceExisting, EvenIfReadOnly, Attributes, Progress );
	if (ReturnValue != COPY_OK)
	{
		ReturnValue = InternalCopy( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(DestFile)), *ConvertToAbsolutePath(SrcFile), ReplaceExisting, EvenIfReadOnly, Attributes, Progress );
	}

	return ReturnValue;
}

UBOOL FFileManagerMPQ::Delete( const TCHAR* Filename, UBOOL RequireExists, UBOOL EvenReadOnly )
{
	// Only allow writes to files that are not signed 
	// Except if the file is missing (that way corrupt ini files can be autogenerated by deleting them)
	if( FSHA1::GetFileSHAHash(Filename, NULL) )
	{
		debugfSuppressed(NAME_DevSHA,TEXT("Can't delete signed game file: %s"),Filename);
		return FALSE;
	}
	
	// we can only delete from user directory
	return InternalDelete( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)), RequireExists, EvenReadOnly );
}

UBOOL FFileManagerMPQ::IsReadOnly( const TCHAR* Filename )
{
	return InternalIsReadOnly( *ConvertToAbsolutePath(Filename) );
}

UBOOL FFileManagerMPQ::Move( const TCHAR* Dest, const TCHAR* Src, UBOOL Replace, UBOOL EvenIfReadOnly, UBOOL Attributes )
{
	// we can only write to user directory, but source may be user or install (try user first)
	UBOOL ReturnValue = InternalMove( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Dest)), *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Src)), Replace, EvenIfReadOnly, Attributes );
	if (ReturnValue == FALSE)
	{
		_DumpFileHandleTraceMap(); // oscar@snda
		ReturnValue = InternalMove( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Dest)), *ConvertToAbsolutePath(Src), Replace, EvenIfReadOnly, Attributes );
	}

	return ReturnValue;
}

UBOOL FFileManagerMPQ::MakeDirectory( const TCHAR* Path, UBOOL Tree )
{
	return InternalMakeDirectory( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Path)), Tree );
}

UBOOL FFileManagerMPQ::DeleteDirectory( const TCHAR* Path, UBOOL RequireExists, UBOOL Tree )
{
	return InternalDeleteDirectory( *ConvertToAbsolutePath(Path), RequireExists, Tree );
}

void FFileManagerMPQ::FindFiles( TArray<FString>& Result, const TCHAR* Filename, UBOOL Files, UBOOL Directories )
{
	// first perform the find in the User directory
	InternalFindFiles( Result, *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)), Files, Directories );
	
	// now do the find in the install directory
	TArray<FString> InstallDirResults;
	InternalFindFiles( InstallDirResults, *ConvertToAbsolutePath(Filename), Files, Directories );

	// now add any new files to the results (so that user dir files override install dir files)
	for (INT InstallFileIndex = 0; InstallFileIndex < InstallDirResults.Num(); InstallFileIndex++)
	{
		Result.AddUniqueItem(*InstallDirResults(InstallFileIndex));
	}
}

DOUBLE FFileManagerMPQ::GetFileAgeSeconds( const TCHAR* Filename )
{
	// first look for the file in the user dir
	DOUBLE ReturnValue = InternalGetFileAgeSeconds( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)) );
	// then look in install dir if it failed to be found
	if (ReturnValue == -1.0)
	{
		ReturnValue = InternalGetFileAgeSeconds( *ConvertToAbsolutePath(Filename) );
	}

	return ReturnValue;
}

DOUBLE FFileManagerMPQ::GetFileTimestamp( const TCHAR* Filename )
{
	// first look for the file in the user dir
	DOUBLE ReturnValue = InternalGetFileTimestamp( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)) );
	// then look in install dir if it failed to be found
	if (ReturnValue == -1.0)
	{
		ReturnValue = InternalGetFileTimestamp( *ConvertToAbsolutePath(Filename) );
	}

	return ReturnValue;
}

UBOOL FFileManagerMPQ::GetTimestamp( const TCHAR* Filename, FTimeStamp& Timestamp )
{
	// first look for the file in the user dir
	UBOOL ReturnValue = InternalGetTimestamp( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)), Timestamp );
	// then look in install dir if it failed to be found
	if (ReturnValue == FALSE)
	{
		ReturnValue = InternalGetTimestamp( *ConvertToAbsolutePath(Filename), Timestamp );
	}

	return ReturnValue;
}


/**
 * Updates the modification time of the file on disk to right now, just like the unix touch command
 * @param Filename Path to the file to touch
 * @return TRUE if successful
 */
UBOOL FFileManagerMPQ::TouchFile(const TCHAR* Filename)
{
	// first look for the file in the user dir
	UBOOL ReturnValue = InternalTouchFile( *ConvertAbsolutePathToUserPath(*ConvertToAbsolutePath(Filename)) );
	// then look in install dir if it failed to be found
	if (ReturnValue == FALSE)
	{
		ReturnValue = InternalTouchFile( *ConvertToAbsolutePath(Filename) );
	}

	return ReturnValue;
}

UBOOL FFileManagerMPQ::GetDiskFreeSpace(const TCHAR* Drive, QWORD& FreeBytesToCaller, QWORD& TotalBytes, QWORD& FreeBytes )
{
	return InternalGetDiskFreeSpace( Drive, FreeBytesToCaller,TotalBytes, FreeBytes );
}

/*-----------------------------------------------------------------------------
	Internal interface
-----------------------------------------------------------------------------*/

HANDLE FFileManagerMPQ::GetArchiveHandleByName(const TCHAR* szArchiveName)
{
	HANDLE hArchive = NULL;

	HANDLE* phArchive = MapMPQHandle.Find(szArchiveName);

	if (phArchive == NULL)
	{
		SFileOpenArchive(szArchiveName, 0, STREAM_PROVIDER_FLAT, &hArchive);

		MapMPQHandle.Set(szArchiveName, hArchive);

		DWORD s;
		SFileGetFileInfo(hArchive, SFileMpqNumberOfFiles, &s, sizeof(s), NULL);
	}
	else
	{
		hArchive = *phArchive;
	}

	return hArchive;
}

HANDLE FFileManagerMPQ::GetArchiveHandleByName(const TCHAR* szFileName, char szPathInMPQ[], UINT nPathInMPQLen)
{
	HANDLE hArchive = NULL;

	//FString fp = appConvertRelativePathToFull(szFileName);
	//RELATIVE_TO_FULL_FUNC();//

	const FMPQPath* pMPQPath = MapFilesToMPQ.Find(szFileName);

	if (pMPQPath)
	{
		hArchive = GetArchiveHandleByName(*pMPQPath->PathMPQ);

		if (szPathInMPQ != NULL)
		{	
			WideCharToMultiByte(CP_ACP,0,*(pMPQPath->PathInMPQ),(DWORD)(pMPQPath->PathInMPQ.Len())+1,szPathInMPQ,nPathInMPQLen,NULL,NULL);
		}
	}

	return hArchive;
}

HANDLE FFileManagerMPQ::GetFileHandleByName(const TCHAR* szNameInMPQ, HANDLE hArchive)
{
	check(szNameInMPQ && hArchive != NULL);

	char szDestFileName[1024];

	HANDLE hFile;

	WideCharToMultiByte(CP_ACP,0,szNameInMPQ,(DWORD)(appStrlen(szNameInMPQ))+1,szDestFileName,sizeof(szDestFileName),NULL,NULL);
	
	SFileOpenFileEx(hArchive, szDestFileName, 0, &hFile);

	return hFile;
}

HANDLE FFileManagerMPQ::GetFileHandleByName(IN const TCHAR* FileName, IN DWORD Flags, IN FOutputDevice* Error, OUT HANDLE& hArchive)
{
	HANDLE hFile = NULL;

	//FString fp = appConvertRelativePathToFull(FileName);
	//RELATIVE_TO_FULL_FUNC();//

	FString f(FileName);

	//if (f.InStr(TEXT("messagebox"), FALSE, TRUE) != -1)
	//{
	//	appDebugBreak();
	//}

	const FMPQPath* pMPQPath = MapFilesToMPQ.Find(FileName);
	
	if (pMPQPath)
	{
		hArchive = GetArchiveHandleByName(*pMPQPath->PathMPQ);

		if (hArchive != NULL)
		{
			hFile = GetFileHandleByName(*pMPQPath->PathInMPQ, hArchive);
		}
	}

	return hFile;
}

FArchive* FFileManagerMPQ::InternalCreateFileReader( const TCHAR* FilenameSrc, DWORD Flags, FOutputDevice* Error )
{
	const TCHAR* Filename = FilenameSrc;
#if M_HASH_FILE_NAME // oscar@snda
	FString FilenameStr = TryFindHashFileName(FilenameSrc);
	Filename = *FilenameStr;
#endif // oscar@snda

	INT StatsHandle = FILE_IO_STATS_GET_HANDLE( Filename );
	SCOPED_FILE_IO_READ_OPEN_STATS( StatsHandle );

	HANDLE hArchive = NULL;
	// The pak copies of the Config tables are an older retail version; prefer the loose (plain) file when present.
	HANDLE hFile = NULL;
	{
		FString LooseName( Filename );
		LooseName = LooseName.ToUpper();
		const UBOOL bConfigFile = LooseName.InStr( TEXT("\\CONFIG\\") ) != INDEX_NONE || LooseName.InStr( TEXT("/CONFIG/") ) != INDEX_NONE;
		if( !bConfigFile || GetFileAttributesW( Filename ) == INVALID_FILE_ATTRIBUTES )
		{
			hFile = GetFileHandleByName(Filename, Flags, Error, hArchive);
		}
	}

	//ConvertToAbsolutePath()
	
	if (hFile)
	{
		DWORD Filesize;
		Filesize = SFileGetFileSize(hFile, NULL);
		//SFileGetFileInfo(hFile, SFileInfoFileSize, &Filesize, sizeof(Filesize), NULL);
		
		FArchive* retArch = new FArchiveFileReaderMPQ(hFile,StatsHandle,Filename,Error,Filesize);

		if( retArch && (Flags & FILEREAD_SaveGame) )
		{
			retArch->SetIsSaveGame( TRUE );
		}
		return retArch;
	}
	else
	{
		DWORD  Access    = GENERIC_READ;
		DWORD  WinFlags  = FILE_SHARE_READ;
		DWORD  Create    = OPEN_EXISTING;
		HANDLE Handle    = CreateFileW( Filename, Access, WinFlags, NULL, Create, FILE_ATTRIBUTE_NORMAL, NULL );
		dbgCreateFile(Handle); // oscar@snda

		if( Handle==INVALID_HANDLE_VALUE )
		{
			if( Flags & FILEREAD_NoFail )
			{
				const DWORD LastError = GetLastError();
				appErrorf( TEXT("Failed to read file: %s, GetLastError %u"), Filename, LastError );
			}
			return NULL;
		}

		TRACE_FILE_HANDLE_ADD(Handle, Filename); // oscar@snda

		FArchive* retArch = new FArchiveFileReaderWindows(Handle,StatsHandle,Filename,Error,GetFileSize(Handle,NULL));
		if( retArch && (Flags & FILEREAD_SaveGame) )
		{
			retArch->SetIsSaveGame( TRUE );
		}
		return retArch;
	}
}

FArchive* FFileManagerMPQ::InternalCreateFileWriter( const TCHAR* FilenameSrc, DWORD Flags, FOutputDevice* Error )
{

	// appOutputDebugStringf(TEXT("InternalCreateFileWriter:%s\n"), Filename); // oscar@snda
	const TCHAR* Filename = FilenameSrc;
#if M_HASH_FILE_NAME // oscar@snda
	FString FilenameStr = TryFindHashFileName(FilenameSrc);
	Filename = *FilenameStr;
#endif // oscar@snda

	INT StatsHandle = FILE_IO_STATS_GET_HANDLE( Filename );
	SCOPED_FILE_IO_WRITE_OPEN_STATS( StatsHandle );

	HANDLE hArchive = NULL;
	HANDLE hFile = GetFileHandleByName(Filename, Flags, Error, hArchive);

	if (hFile)
	{
		DWORD Filesize;
		Filesize = SFileGetFileSize(hFile, NULL);
		//SFileGetFileInfo(hFile, SFileInfoFileSize, &Filesize, sizeof(Filesize), NULL);

		INT    Pos       = 0;
		if( Flags & FILEWRITE_Append )
		{
			Pos = SFileSetFilePointer(hFile, 0, NULL, FILE_END);
		}
		FArchive* retArch = new FArchiveFileWriterMPQ(hFile,StatsHandle,Filename,Error,Pos);

		if( retArch && (Flags & FILEREAD_SaveGame) )
		{
			retArch->SetIsSaveGame( TRUE );
		}
		return retArch;
	}
	else
	{
		MakeDirectory(*FFilename(Filename).GetPath(), TRUE);

		if( (FileSize (Filename) >= 0) && (Flags & FILEWRITE_EvenIfReadOnly) )
		{
			SetFileAttributesW(Filename, 0);
		}
		DWORD  Access    = GENERIC_WRITE;
		DWORD  WinFlags  = (Flags & FILEWRITE_AllowRead) ? FILE_SHARE_READ : 0;
		DWORD  Create    = (Flags & FILEWRITE_Append) ? OPEN_ALWAYS : (Flags & FILEWRITE_NoReplaceExisting) ? CREATE_NEW : CREATE_ALWAYS;
		HANDLE Handle    = CreateFileW( Filename, Access, WinFlags, NULL, Create, FILE_ATTRIBUTE_NORMAL, NULL );
		dbgCreateFile(Handle); // oscar@snda
		if( Handle==INVALID_HANDLE_VALUE )
		{
			if( Flags & FILEWRITE_NoFail )
			{
				const DWORD LastError = GetLastError();
				appErrorf( TEXT("Failed to create file: %s, GetLastError %u"), Filename, LastError );
			}
			return NULL;
		}

		TRACE_FILE_HANDLE_ADD(Handle, Filename); // oscar@snda

		INT    Pos       = 0;
		if( Flags & FILEWRITE_Append )
		{
			Pos = SetFilePointer( Handle, 0, NULL, FILE_END );
		}
		FArchive* retArch = new FArchiveFileWriterWindows(Handle,StatsHandle,Filename,Error,Pos);
		if( retArch && (Flags & FILEWRITE_SaveGame) )
		{
			retArch->SetIsSaveGame( TRUE );
		}
		return retArch;
	}
}

INT FFileManagerMPQ::InternalFileSize( const TCHAR* FilenameSrc )
{
	const TCHAR* Filename = FilenameSrc;
#if M_HASH_FILE_NAME // oscar@snda
	FString FilenameStr = TryFindHashFileName(FilenameSrc);
	Filename = *FilenameStr;
#endif // oscar@snda

	HANDLE hArchive = NULL;
	HANDLE hFile = GetFileHandleByName(Filename, 0, NULL, hArchive);

	if (hFile != NULL)
	{
		DWORD Filesize;

		Filesize = SFileGetFileSize(hFile, NULL);

		SFileCloseFile(hFile);

		//TRACE_FILE_HANDLE_REMOVE(hFile); // oscar@snda

		return Filesize;
	}
	else
	{
		HANDLE Handle = CreateFileW( Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		dbgCreateFile(Handle); // oscar@snda

		if( Handle==INVALID_HANDLE_VALUE )
		{
			return -1;
		}

		TRACE_FILE_HANDLE_ADD(Handle, Filename); // oscar@snda

		DWORD Result = GetFileSize( Handle, NULL );
		BOOL hr = CloseHandle( Handle ); // oscar@snda
		dbgCloseFile(Filename, Handle, hr); // oscar@snda
		TRACE_FILE_HANDLE_REMOVE(Handle); // oscar@snda
		return Result;
	}
}

DWORD FFileManagerMPQ::InternalCopy( const TCHAR* DestFile, const TCHAR* SrcFile, UBOOL ReplaceExisting, UBOOL EvenIfReadOnly, UBOOL Attributes, FCopyProgress* Progress )
{
	if( EvenIfReadOnly )
	{
		SetFileAttributesW(DestFile, 0);
	}
	DWORD Result;
	if( Progress )
	{
		Result = FFileManagerGeneric::Copy( DestFile, SrcFile, ReplaceExisting, EvenIfReadOnly, Attributes, Progress );
	}
	else
	{
		HANDLE hArchive = NULL;
		check(GetFileHandleByName(DestFile, 0, NULL, hArchive) == NULL && GetFileHandleByName(SrcFile, 0, NULL, hArchive) == NULL);// zhaojun@snda

		MakeDirectory(*FFilename(DestFile).GetPath(), TRUE);

		if( CopyFileW(SrcFile, DestFile, !ReplaceExisting) != 0)
		{
			Result = COPY_OK;
		}
		else
		{
			Result = COPY_MiscFail;
		}
	}

	if( Result==COPY_OK && !Attributes )
	{
		SetFileAttributesW(DestFile, 0);
	}
	return Result;
}

UBOOL FFileManagerMPQ::InternalDelete( const TCHAR* Filename, UBOOL RequireExists, UBOOL EvenReadOnly )
{
	if( EvenReadOnly )
	{
		SetFileAttributesW(Filename,FILE_ATTRIBUTE_NORMAL);
	}
	
	// begin : zhaojun@snda
	if (wcsstr(Filename, TEXT(".exe")) != NULL
		|| wcsstr(Filename, TEXT(".EXE")) != NULL
		|| wcsstr(Filename, TEXT(".dll")) != NULL
		|| wcsstr(Filename, TEXT(".DLL")) != NULL)
	{
		debugf( TEXT("Could not delete '%s',zhaojun@snda"), Filename);
		debugf( TEXT("Could not delete '%s',zhaojun@snda"), Filename);
		debugf( TEXT("Could not delete '%s',zhaojun@snda"), Filename);

		return 0;
	}
	// ~end~ : zhaojun@snda

	char szFileName[1024];
	HANDLE hArchive = GetArchiveHandleByName(Filename, szFileName, sizeof(szFileName));

	if (hArchive)
	{
		return SFileRemoveFile(hArchive, szFileName, 0);
	}
	else
	{
		INT Result = DeleteFile(Filename) != 0;
		DWORD Error = GetLastError();
		Result = Result || (!RequireExists && (Error==ERROR_FILE_NOT_FOUND || Error==ERROR_PATH_NOT_FOUND));
		if( !Result )
		{
			if ((appStristr(appCmdLine(), TEXT("CookPackages")) != NULL) || GIsCooking || ParseParam(appCmdLine(), TEXT("MTCHILD")) || ParseParam(appCmdLine(), TEXT("MT")))
			{
				// this is not an error while doing MT commandlets
				// the cooker will retry and fail gracefully if there is a true issue
				debugf( TEXT("Could not delete '%s'"), Filename);
			}
			else
			{
				DWORD error = GetLastError();
				debugf( NAME_Error, TEXT("Error deleting file '%s' (GetLastError: %d)"), Filename, error );
			}
		}
		return Result!=0;
	}


	//return TRUE; it's ok now??? // oscar@snda
}

UBOOL FFileManagerMPQ::InternalIsReadOnly( const TCHAR* Filename )
{
	DWORD rc;
	if( FileSize( Filename ) < 0 )
	{
		return( 0 );
	}

	HANDLE hArchive = GetArchiveHandleByName(Filename, NULL, 0);

	if (hArchive != NULL)
	{
		INT32 ReadOnly;
		SFileGetFileInfo(hArchive, SFileMpqIsReadOnly, &ReadOnly, sizeof(ReadOnly), NULL);
		return (ReadOnly != 0);
	}
	else
	{
		rc = GetFileAttributesW(Filename);
		
		if (rc != 0xFFFFFFFF)
		{
			return ((rc & FILE_ATTRIBUTE_READONLY) != 0);
		}
		else
		{
			debugf( NAME_Error, TEXT("Error reading attributes for '%s'"), Filename );
			return (0);
		}
	}
}

UBOOL FFileManagerMPQ::InternalMove( const TCHAR* Dest, const TCHAR* Src, UBOOL Replace, UBOOL EvenIfReadOnly, UBOOL Attributes )
{
	if (appStrstr(Dest, PAK_EXT_WITH_POINT) != NULL)
	{
		check(0);

		return 0;
	}
	else
	{
		MakeDirectory(*FFilename(Dest).GetPath(), TRUE);
		INT DeleteResult = DeleteFile(Dest);
		DWORD error = 0;
		// Retry on failure, unless the file wasn't there anyway.
		if(DeleteResult == 0 && ( error = GetLastError() ) != ERROR_FILE_NOT_FOUND )
		{
			// If the delete failed, throw a warning but retry before we throw an error
			warnf( NAME_Warning, TEXT("DeleteFile was unable to delete '%s', retrying... (GetLastE-r-r-o-r: %d)"), Dest, error );

			// Wait just a little bit (i.e. a totally arbitrary amount)...
			Sleep(500);

			// Try again
			DeleteResult = DeleteFile(Dest);
			if(DeleteResult == 0)
			{
				error = GetLastError();
				warnf( NAME_Error, TEXT("Error deleting file '%s' (GetLastError: %d)"), Dest, error );
				return FALSE;
			}
			else
			{
				warnf( NAME_Warning, TEXT("DeleteFile recovered during retry!") );
			}		
		}

		INT MoveResult = MoveFile(Src,Dest);
		if(MoveResult == 0)
		{
			// If the move failed, throw a warning but retry before we throw an error
			DWORD error = GetLastError();
			warnf( NAME_Warning, TEXT("MoveFileExW was unable to move '%s' to '%s', retrying... (GetLastE-r-r-o-r: %d)"), Src, Dest, error );

			// Wait just a little bit (i.e. a totally arbitrary amount)...
			Sleep(500);

			// Try again
			MoveResult = MoveFile(Src,Dest);
			if(MoveResult == 0)
			{
				error = GetLastError();
				warnf( NAME_Error, TEXT("Error moving file '%s' to '%s' (GetLastError: %d)"), Src, Dest, error );
				return FALSE;
			}
			else
			{
				warnf( NAME_Warning, TEXT("MoveFileExW recovered during retry!") );
			}
		}
		return (MoveResult != 0);
	}
}

UBOOL FFileManagerMPQ::InternalMakeDirectory( const TCHAR* Path, UBOOL Tree )
{
	if( Tree )
	{
		return FFileManagerGeneric::MakeDirectory( Path, Tree );
	}

	// it's possible that the path exists, but the user doesn't have access to create it,
	// so CreateDirectory() would return ERROR_ACCESS_DENIED instead of ERROR_ALREADY_EXISTS.
	// since ERROR_ACCESS_DENIED is a valid error for a non-existent path, we want to allow both scenarios to return properly.
	DWORD PathAttributes = GetFileAttributesW(Path);
	if ((PathAttributes != INVALID_FILE_ATTRIBUTES) && ((PathAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
	{
		return TRUE;
	}
	else
	{
	return CreateDirectoryW(Path,NULL)!=0 || GetLastError()==ERROR_ALREADY_EXISTS;
}
}

UBOOL FFileManagerMPQ::InternalDeleteDirectory( const TCHAR* Path, UBOOL RequireExists, UBOOL Tree )
{
	if( Tree )
		return FFileManagerGeneric::DeleteDirectory( Path, RequireExists, Tree );
	return RemoveDirectoryW(Path)!=0 || (!RequireExists && GetLastError()==ERROR_FILE_NOT_FOUND);
}

void FFileManagerMPQ::InternalFindFiles( TArray<FString>& Result, const TCHAR* Filename, UBOOL Files, UBOOL Directories )
{
	HANDLE Handle=NULL;
	WIN32_FIND_DATAW Data;
	Handle=FindFirstFileW(Filename,&Data);
	if( Handle!=INVALID_HANDLE_VALUE )
	{
		do
		{
			if
			(   appStricmp(Data.cFileName,TEXT("."))
			&&  appStricmp(Data.cFileName,TEXT(".."))
			&&  ((Data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)?Directories:Files) )
			{
				new(Result)FString(Data.cFileName);
			}
		}
		while( FindNextFileW(Handle,&Data) );
	}
	if( Handle!=INVALID_HANDLE_VALUE )
	{
		FindClose( Handle );
	}
}

DOUBLE FFileManagerMPQ::InternalGetFileAgeSeconds( const TCHAR* FilenameSrc )
{
	const TCHAR* Filename = FilenameSrc;
#if M_HASH_FILE_NAME // oscar@snda
	FString FilenameStr = TryFindHashFileName(FilenameSrc);
	Filename = *FilenameStr;
#endif // oscar@snda

	DOUBLE ReturnValue = -1.0;
	HANDLE FileHandle = CreateFile( Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	dbgCreateFile(FileHandle); // oscar@snda
	if( FileHandle != INVALID_HANDLE_VALUE )
	{
		TRACE_FILE_HANDLE_ADD(FileHandle, Filename); // oscar@snda

		// Get the system time as a file time
		SYSTEMTIME SystemTimeRaw;
		FILETIME SystemTimeFileTime;
		GetSystemTime( &SystemTimeRaw );
		SystemTimeToFileTime( &SystemTimeRaw, &SystemTimeFileTime );

		// Get the file time
		FILETIME FileCreationTime, FileLastAccessTime, FileLastWriteTime;
		GetFileTime( FileHandle, &FileCreationTime, &FileLastAccessTime, &FileLastWriteTime );

		// Compare
		LARGE_INTEGER SystemTimeQuad, FileTimeQuad;
		SystemTimeQuad.LowPart = SystemTimeFileTime.dwLowDateTime;
		SystemTimeQuad.HighPart = SystemTimeFileTime.dwHighDateTime;
		FileTimeQuad.LowPart = FileLastWriteTime.dwLowDateTime;
		FileTimeQuad.HighPart = FileLastWriteTime.dwHighDateTime;

		// Time is in 100-nanosecond units
		ReturnValue = (DOUBLE)( SystemTimeQuad.QuadPart - FileTimeQuad.QuadPart ) / 10000000.0;

		BOOL hr = CloseHandle( FileHandle ); // oscar@snda
		dbgCloseFile(Filename, FileHandle, hr); // oscar@snda
		TRACE_FILE_HANDLE_REMOVE(FileHandle); // oscar@snda
	}
	return ReturnValue;
}

DOUBLE FFileManagerMPQ::InternalGetFileTimestamp( const TCHAR* Filename )
{
	struct _stat FileInfo;
	if( 0 == _wstat( Filename, &FileInfo ) )
	{
		time_t FileTime;	
		FileTime = FileInfo.st_mtime;
		return FileTime;
	}
	return -1.0;
}

UBOOL FFileManagerMPQ::InternalGetTimestamp( const TCHAR* Filename, FTimeStamp& Timestamp )
{
	appMemzero( &Timestamp, sizeof(Timestamp) );
	struct _stat FileInfo;
	if( 0 == _wstat( Filename, &FileInfo ) )
	{
		time_t	FileTime;	
		FileTime = FileInfo.st_mtime;
#if USE_SECURE_CRT
		tm Time;
		gmtime_s(&Time,&FileTime);
#else
		tm& Time = *gmtime(&FileTime);
#endif
		Timestamp.Day       = Time.tm_mday;
		Timestamp.Month     = Time.tm_mon;
		Timestamp.DayOfWeek = Time.tm_wday;
		Timestamp.Hour      = Time.tm_hour;
		Timestamp.Minute    = Time.tm_min;
		Timestamp.Second    = Time.tm_sec;
		Timestamp.Year      = Time.tm_year + 1900;
		return TRUE;
	}
	return FALSE;
}

/**
 * Updates the modification time of the file on disk to right now, just like the unix touch command
 * @param Filename Path to the file to touch
 * @return TRUE if successful
 */
UBOOL FFileManagerMPQ::InternalTouchFile(const TCHAR* Filename)
{
	time_t Now;
	// get the current time
	time(&Now);

	// use now as the time to set
	_utimbuf Time;
	Time.modtime = Now;
	Time.actime = Now;

	// set it to the file
	INT ReturnCode = _wutime(Filename, &Time);

	// 0 return code was a success
	return ReturnCode == 0;
}

/**
 *	Sets the timestamp of a file.
 *
 *	@param Filename		Full path to the file.
 *	@param Timestamp	Timestamp to set
 *	@return				File size in bytes or -1 if the file didn't exist.
 **/
UBOOL FFileManagerMPQ::InternalSetFileTimestamp( const TCHAR* Filename, DOUBLE TimeStamp )
{
	_utimbuf Time;
	Time.modtime = TimeStamp;
	Time.actime = TimeStamp;

	// set it to the file
	INT ReturnCode = _wutime(Filename, &Time);

	// 0 return code was a success
	return ReturnCode == 0;
}

/**
 *	Truncates an existing file, discarding data at the end to make it smaller. (Thread-safe)
 *
 *	@param Filename		Full path to the file.
 *	@param FileSize		New file size to truncate to. If this is larger than current file size, the function won't do anything.
 *	@return				Resulting file size or INDEX_NONE if the file didn't exist.
 **/
INT FFileManagerMPQ::InternalFileTruncate( const TCHAR* FilenameSrc, INT FileSize )
{
	const TCHAR* Filename = FilenameSrc;
#if M_HASH_FILE_NAME // oscar@snda
	FString FilenameStr = TryFindHashFileName(FilenameSrc);
	Filename = *FilenameStr;
#endif // oscar@snda

	INT NewFileSize = INDEX_NONE;
	HANDLE WinHandle = CreateFile( Filename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	dbgCreateFile(WinHandle); // oscar@snda
	UBOOL bSuccess = (WinHandle != INVALID_HANDLE_VALUE);
	if ( bSuccess )
	{
		TRACE_FILE_HANDLE_ADD(WinHandle, Filename); // oscar@snda

		DWORD CurrentFileSize = GetFileSize( WinHandle, NULL );
		NewFileSize = INT(CurrentFileSize);
		UBOOL bSuccess = (CurrentFileSize != INVALID_FILE_SIZE);
		if ( bSuccess && CurrentFileSize > DWORD(FileSize) )
		{
			DWORD NewPosition = SetFilePointer( WinHandle, FileSize, NULL, FILE_BEGIN );
			bSuccess = bSuccess && NewPosition == DWORD(FileSize) && SetEndOfFile( WinHandle );
			NewFileSize = FileSize;
		}

		BOOL hr = CloseHandle( WinHandle ); // oscar@snda
		dbgCloseFile(Filename, WinHandle, hr); // oscar@snda
		TRACE_FILE_HANDLE_REMOVE(WinHandle); // oscar@snda
	}
	return bSuccess ? NewFileSize : INDEX_NONE;
}

UBOOL FFileManagerMPQ::InternalGetDiskFreeSpace(const TCHAR* Drive, QWORD& FreeBytesToCaller, QWORD& TotalBytes, QWORD& FreeBytes )
{
	QWORD LocalFreeBytesToCaller;
	QWORD LocalTotalBytes;
	QWORD LocalFreeBytes;

	UBOOL Result = GetDiskFreeSpaceEx( Drive, (PULARGE_INTEGER)&LocalFreeBytesToCaller, (PULARGE_INTEGER)&LocalTotalBytes, (PULARGE_INTEGER)&LocalFreeBytes );

	if( Result )
	{
		FreeBytesToCaller = LocalFreeBytesToCaller;
		TotalBytes = LocalTotalBytes;
		FreeBytes = LocalFreeBytes;
	}

	return Result;
}

INT FFileManagerMPQ::EnumFilesInPackage(const TCHAR* PackageName, 
	TArray<FString>& FilesList, UBOOL bListPackage, UBOOL bListNonPackage, 
	TMap<FString, FMPQPath>& FilesToMPQMap, UBOOL bGenerateMapping)
{
	HANDLE hArchive = NULL;

	SFileOpenArchive(PackageName, 0, STREAM_PROVIDER_FLAT, &hArchive);

	INT nCount = 0;

	if (hArchive)
	{
		SFILE_FIND_DATA fd;
		HANDLE hFind = SFileFindFirstFile(hArchive, "*", &fd, NULL);

		if (hFind)
		{
			FString strPath(PackageName);
			FString strDirectory = strPath.LeftChop(PAK_NAMELEN);

			check(*(*strDirectory + strDirectory.Len() - 1) == TEXT('\\') || 
				*(*strDirectory + strDirectory.Len() - 1) == TEXT('/'));

			//strDirectory += TEXT("\\");

			TCHAR result[MAX_PATH];

			do
			{
				if (strcmp(fd.cFileName, "(listfile)") != 0 && strcmp(fd.cFileName, "(attributes)") != 0)
				{
					MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS,(LPCSTR)fd.cFileName,-1,result,MAX_PATH);

					FString strFile(strDirectory);
					strFile += result;
					FFilename strFileExt(result);

					UBOOL bIsPackage = GSys && GSys->Extensions.FindItemIndex(*strFileExt.GetExtension()) != INDEX_NONE;

					if ((bListNonPackage && !bIsPackage) || (bListPackage && bIsPackage))
					{
						FilesList.AddItem(strFile);
					}

					if (bGenerateMapping)
					{
						check(FilesToMPQMap.Find(strFile) == NULL);
						FilesToMPQMap.Set(strFile, FMPQPath(strPath, fd.cFileName));
					}

					++nCount;
				}
			}while (SFileFindNextFile(hFind, &fd));

			SFileFindClose(hFind);
		}

		SFileCloseArchive(hArchive);
	}

	return nCount;
}

INT FFileManagerMPQ::EnumFilesInPackage(const TCHAR* PackageName, 
	TArray<FString>& FilesList, UBOOL bListPackage, UBOOL bListNonPackage, 
	UBOOL bGenerateMapping)
{
	check(MapMPQHandle.Find(PackageName) == NULL);

	return FFileManagerMPQ::EnumFilesInPackage(PackageName,
		FilesList, bListPackage, bListNonPackage, 
		MapFilesToMPQ, bGenerateMapping);
}

INT FFileManagerMPQ::GenerateMapFromFileToMPQ(const TCHAR* FolderName, 
	TMap<FString, FMPQPath>& FilesToMPQMap)
{
	TArray<FString> Files;

	appFindFilesInDirectory(Files, FolderName, TRUE, TRUE);

	INT nCount = 0;

	for (INT FileIndex = 0; FileIndex < Files.Num(); FileIndex++)
	{
		const TCHAR* InFileName = *Files(FileIndex);

		if (appStrstr(InFileName, PAK_EXT_WITH_POINT) != NULL)
		{
			nCount += EnumFilesInPackage(InFileName, *((TArray<FString>*)(NULL)), FALSE, FALSE, FilesToMPQMap, TRUE);
		}
	}

	return nCount;
}

INT FFileManagerMPQ::GenerateMapFromFileToMPQ( const TCHAR* FolderName )
{
	return GenerateMapFromFileToMPQ(FolderName, MapFilesToMPQ);
}

//UBOOL FFileManagerMPQ::ExtractArchiveAndFileName(const TCHAR* FullPath, 
//	TCHAR ArchiveName[], UINT ArchiveNameBuffSize, char FileName[], UINT FileNameBuffSize,
//	UBOOL bOnlyExtractArchiveName/* = FALSE*/)
//{
//	
//	const TCHAR* KeyPath = TEXT("MMOContent");
//	UINT KeyPathLen = 10;
//
//	check(FullPath && KeyPath && ArchiveName && (FileName || bOnlyExtractArchiveName));
//
//	const TCHAR* p = appStrstr((TCHAR*)FullPath, KeyPath);
//
//	if (p != NULL)
//	{
//		p += KeyPathLen;	//strlen of KeyPath
//
//		if (*p == TEXT('\0'))
//		{
//			return FALSE;
//		}
//
//		p += 1;				// '/' or '\'
//
//		if (*p == TEXT('\0'))
//		{
//			return FALSE;
//		}
//
//		const TCHAR* pNew = appStrchr(p, TEXT('\\'));
//
//		if (pNew == NULL)
//		{
//			p = appStrchr(p, TEXT('/'));
//		}
//		else
//		{
//			p = pNew;
//		}
//
//		if (p == NULL)
//		{
//			return FALSE;
//		}
//
//		check((p-FullPath)*sizeof(TCHAR) + PAK_EXT_WITH_POINT_SIZE < ArchiveNameBuffSize);
//
//		TCHAR* pHeader = ArchiveName;
//		UINT nLen = (p-FullPath);
//		UINT nSize = nLen * sizeof(TCHAR);
//		memcpy_s(pHeader, ArchiveNameBuffSize, FullPath, nSize);
//
//		pHeader += nLen;
//		memcpy_s(pHeader, ArchiveNameBuffSize - nSize, PAK_EXT_WITH_POINT, PAK_EXT_WITH_POINT_SIZE);
//
//		if (!bOnlyExtractArchiveName)
//		{
//			p += 1;	// p is '/' or '\'
//			WideCharToMultiByte(CP_ACP,0,p,(DWORD)lstrlenW(p)+1,FileName,FileNameBuffSize,NULL,NULL);
//		}
//
//		return TRUE;
//	}
//
//	return FALSE;
//
//}

UBOOL FFileManagerMPQ::ParsePAKFileMap(TMap<FString, FMPQPath>& PathMap, const TCHAR* PackageName)
{
	return FALSE;
}

UBOOL FFileManagerMPQ::LoadPAKFileMap(TMap<FString, FMPQPath>& PathMap, const TCHAR* FileName)
{
	return FALSE;
}

class FMPQInfo
{
public:
	HANDLE hArchive;
	UINT size;
	TArray<FString> files;
	TArray<FString> pathsInMPQ;

	FMPQInfo()
	{
		hArchive = NULL;
		size = 0;
	}

	void AddFile(const TCHAR* pFile, const TCHAR* pRelativePathInMPQ, UINT nFileSize = 0)
	{
		check(pFile && pRelativePathInMPQ);

		files.AddItem(pFile);
		pathsInMPQ.AddItem(pRelativePathInMPQ);

		check(0 == nFileSize || nFileSize == GFileManager->FileSize(pFile));

		size += (nFileSize == 0 ? GFileManager->FileSize(pFile) : nFileSize);
	}
};

void refineFileNames(TArray<FString>& DestFiles, TArray<FString>& SrcFiles, const TMap<FString, FFileManagerMPQ::FMPQPath>& PathMap)
{
	if (PathMap.Num() == 0)
	{
		if (&DestFiles == &SrcFiles)
		{
			// do nothing
		}
		else
		{
			// copy, to do
			check(0);
		}
	}
	else
	{
		check(&DestFiles != &SrcFiles);

		// put the old file at the head of array
		// put the new file at the tail of array
		// to do
		check(0);
	}
}

UBOOL FFileManagerMPQ::PackFolder(
	const TCHAR* FolderName, const TArray<FString>* ArrayExcludeFiles,
	TMap<FString, FMPQPath>& RefFilesMap, 
	UINT expectedSize/* =  1024*1024*1024*/, UINT maxSize/* = 2048*1024*1024*/, 
	UBOOL bDelSrc/* = FALSE*/)
{
	check(FolderName && *FolderName != 0);
	
	// don't end with / and '\\'
	check((*(FolderName+appStrlen(FolderName)-1)!=TEXT('/'))&&(*(FolderName+appStrlen(FolderName)-1)!=TEXT('\\')));

	TArray<FString> files;

	appFindFilesInDirectory(files, FolderName, TRUE, TRUE);

	refineFileNames(files, files, RefFilesMap);

	TMap<FString, FMPQInfo>	MPQsMap;

	for (INT f  = 0; f < files.Num(); ++f)
	{
		FString FileName(*files(f));

		check(appStrcmp(*(FFilename(FileName).GetExtension()), PAK_EXT) != 0);	// mpq contain mpq, forbidden

		FString MPQPath;
		FString strPathInMPQ;
		FString strPathInMPQExist;

		strPathInMPQ = FileName.RightChop(appStrlen(FolderName) + 1);

		const FMPQPath* pMPQPath = RefFilesMap.Find(FileName);

		if (pMPQPath && appStrstr(*pMPQPath->PathMPQ, FolderName) != NULL 
			&& *((*pMPQPath->PathMPQ) + appStrlen(FolderName)) != TEXT('\\')
			&& *((*pMPQPath->PathMPQ) + appStrlen(FolderName)) != TEXT('/'))
		{
			// 1. if the old path exist and in the same root directory, VALID
			// so first try to package the file into the suggested mpq
			MPQPath = pMPQPath->PathMPQ;
			strPathInMPQExist = pMPQPath->PathInMPQ;
		}
		else
		{
			// new file, since last version
			// old file, but different root path
		}

		UINT nIndex = 0;
		// 
		while (1)
		{
			if (MPQPath.Len() > 0)
			{
				FMPQInfo* pMPQInfo = MPQsMap.Find(MPQPath);

				if (pMPQInfo == NULL)
				{
					// new package, must big enough to hold the file
					pMPQInfo = &(MPQsMap.Set(MPQPath, FMPQInfo()));
				}

				check(pMPQInfo);

				UINT nFileSize = (UINT)(GFileManager->FileSize(*FileName));
				UINT nEvaluatedSize = pMPQInfo->size + nFileSize;
				UINT nLimitedSize = (nIndex == 0 ? maxSize : expectedSize);

				if (nEvaluatedSize <= nLimitedSize )
				{
					// cache it
					pMPQInfo->AddFile(*FileName, *strPathInMPQ, nFileSize);

					// need pack it imm? and then we know the exact size
					MPQPath; FileName; strPathInMPQ;

					break;
				}
				else
				{
					check(nFileSize <= nLimitedSize);
				}
			}

			MPQPath = FolderName;
			MPQPath += TEXT('\\');

			TCHAR szTemp[64];
			swprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), PAK_BASENAME_FORMAT, nIndex);
			MPQPath += szTemp;

			MPQPath += PAK_EXT_WITH_POINT;
			++nIndex;
		}
	}

	const float FileCountCoefficient = 2;
	for (TMap<FString, FMPQInfo>::TIterator itor(MPQsMap); itor; ++itor)
	{
		const FString& rKey = itor.Key();
		FMPQInfo& rInfo = itor.Value();

		HANDLE hArchive = NULL;
		SFileCreateArchive(*rKey, 0, rInfo.files.Num(), &hArchive);
		rInfo.hArchive = hArchive;

		if (hArchive)
		{
			for (INT i = 0; i < rInfo.files.Num(); ++i)
			{
				const FString& SrcFileName = rInfo.files(i);
				const FString& DestFileName = rInfo.pathsInMPQ(i);

				char szDestFileName[1024];
				WideCharToMultiByte(CP_ACP,0,*DestFileName,(DWORD)(DestFileName.Len()+1),szDestFileName,sizeof(szDestFileName),NULL,NULL);

				SFileAddFileEx(hArchive, *SrcFileName, szDestFileName, MPQ_FILE_REPLACEEXISTING, 0, 0);

				// add the file-map to file, then used for client;
				// precache it when start-up

			}

			SFileCloseArchive(hArchive);
		}
		else
		{
			int nErrorCode = GetLastError();

			switch (nErrorCode)
			{
			case 183:
				// already exist
				break;
			}
		}
	}

	return (MPQsMap.Num() > 0);
}

//UBOOL FFileManagerMPQ::PackFiles(TArray<FString>& files, TMap<FString, FMPQPath>& PathMap, 
//	UINT expectedSize/* =  1024*1024*1024*/, UINT maxSize/* = 2048*1024*1024*/, 
//	UBOOL bDelSrc/* = FALSE*/)
//{
//
//}

UBOOL FFileManagerMPQ::PackFiles(const FString& destPackage, TArray<FString>& Srcfiles, UBOOL bDelSrc/* = FALSE*/)
{
	return TRUE;
}




#if M_TRACE_FILE_HANDLE // oscar@snda

void FFileManagerMPQ::_AddToTraceMap( FTraceFileHandleInfo info )
{
	if(TraceFHMap.HasKey(info.Handle))
	{
		//appDebugBreak();
	}
	else
	{
		TraceFHMap.Set(info.Handle, info);
	}

	if(info.FileName.InStr(TEXT("LocalShaderCache-PC-D3D-SM3"), TRUE) != -1)
	{
		appDebugBreak();
	}
}
//------------------------------------------------------------------------
void FFileManagerMPQ::_RemoveFromTraceMap( HANDLE hdl )
{
	if(!TraceFHMap.HasKey(hdl))
	{
		//appDebugBreak();
	}
	else
	{
		TraceFHMap.Remove(hdl);
	}
}
//------------------------------------------------------------------------
void FFileManagerMPQ::_DumpFileHandleTraceMap()
{
	TraceFHMap.Dump(*GLog);
}

#endif // M_TRACE_FILE_HANDLE  // oscar@snda
#endif

#endif	// WITH_STORMLIB	// zhaojun@snda


// PakPatcher.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../PatchPath.h"

#include <assert.h>
#include <string.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <new>
#include <memory>

#include "StormPort.h"
#include "StormLib.h"

#include <DbgHelp.h>

#ifdef _DEBUG
#define check(express) assert(express)
#else
#define check(express) if (!(express)) OutputInfo(BuildinLog, "%s\n", #express);
#endif

#define PAK_EXT	("pak")
#define PAK_EXT_WITH_POINT	(".pak")

#define PAK_EXT_SIZE sizeof(PAK_EXT)
#define PAK_EXT_WITH_POINT_SIZE sizeof(PAK_EXT_WITH_POINT)

#define PAK_BASENAME_FORMAT ("%05d")
#define PAK_FULLNAME_FORMAT ("%05d.pak")
#define PAK_BASENAME_LEN 5
#define PAK_NAME_LEN (5+4)

#define DELTA_EXT_WITH_POINT (".delta")
#define DELTA_EXT_WITH_POINT_SIZE (sizeof(DELTA_EXT_WITH_POINT))

#define PAK_CREATE_FLAG (MPQ_FILE_REPLACEEXISTING|MPQ_FILE_COMPRESS/*|MPQ_FILE_ENCRYPTED*/)
#define PAK_COMPRESSION MPQ_COMPRESSION_ZLIB
#define PAK_COMPRESSION_NEXT 0

// basename len must be equal PAK_BASENAME_LEN;
#define PATCH_DEL_RENAME_TABLE_PAK	("_DRN_.pak")

#define PATCH_DEL_TABLE_FILE	("_DEL_.dat")
#define PATCH_DEL_TABLE_FILE_MAGIC ("MLED")
#define PATCH_DEL_TABLE_FILE_VER ("1.0.0.0")

#define PATCH_RENAME_TABLE_FILE	("_REN_.dat")
#define PATCH_RENAME_TABLE_FILE_MAGIC ("MNER")
#define PATCH_RENAME_TABLE_FILE_VER ("1.0.0.0")

#define STR_DIRECT_FILE_SUB_ITEM_KEY_ATTRIB	("Key")
#define STR_DIRECT_FILE_SUB_ITEM_VALUE_ATTRIB ("Value")

#define PAK_SIZE_THRESHOLD (1024*1024*1024)

typedef size_t (WINAPI* PFNMergeDirInfoProc)(LPCWSTR lpszMergeInfo, int nRetCode);
typedef bool (WINAPI *pfMergeFile)(LPCWSTR lpszSrcFilePath, LPCWSTR lpszDeltaFilePath, LPCWSTR lpszDestFilePath);
typedef bool (WINAPI *pfMergeDir)(LPCWSTR lpszSrcDir, LPCWSTR lpszDeltaDir, LPCWSTR lpszDestDir, PFNMergeDirInfoProc pfn);

HMODULE hXDelta3 = NULL;
pfMergeFile pfMF = NULL;
pfMergeDir pfMD = NULL;

class PathString;

std::vector<PathString> NonePakSubDirs;
std::vector<PathString> PakSubDirs;

std::map<std::string, std::string>* pDelFileList;
std::map<std::string, std::string> DelFileList;

std::map<std::string, std::string>* pResultMD5List;
std::map<std::string, std::string> ResultMD5List;

HANDLE g_hConsole = NULL;

void CreateConsole()
{
	if (g_hConsole == NULL)
	{
		AllocConsole();
		SetConsoleTitle("CQYH……");
		CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		COORD Size;
		Size.X = 200;
		Size.Y = 1000;
		SetConsoleScreenBufferSize(g_hConsole, Size);
	}
}

void DestroyConsole()
{
	if (g_hConsole)
	{
		FreeConsole();
	}
}

size_t WINAPI BuildinLog(LPCWSTR lpszMergeInfo, int nRetCode)
{
	if (g_hConsole)
	{
		DWORD dwWrite = wcslen(lpszMergeInfo);
		WriteConsoleW(g_hConsole, lpszMergeInfo, dwWrite, &dwWrite, NULL);
		return dwWrite;
	}
	else
	{
		wprintf(L"%ls", lpszMergeInfo);
		return 0;
	}
}

void OutputInfo(PFNMergeDirInfoProc pfn, const char* pszFormat, ...)
{
	if (pfn && pszFormat)
	{
		va_list args;
		va_start(args, pszFormat);

		char szTemp[1024] = {0};

		_vsnprintf_s(szTemp, sizeof(szTemp), _TRUNCATE, pszFormat, args);

		va_end(args);

		WCHAR szOutput[1024];

		if (!MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS,szTemp,-1,szOutput, (int)(sizeof(szOutput) / sizeof(szOutput[0]))))
			return;

		(*pfn)(szOutput, 0);
	}
}

// 1. definitions

#include "PakPatcher.inl"

// 目录分隔尽量使用\, 结尾不要以/及\结尾
bool PackFolder(const char* SrcFolderName/*资源位置*/, const char* DestPackageFolder/*mpq位置*/,
	const char* RefFolderName/* = NULL*//*比对mpq*/, const char* DestPatchFolder/* = NULL*//*补丁位置*/, const char* PatchPostfix/* = NULL*//*补丁文件名后缀*/,
	UINT expectedSize = 1073741824/*正常大小1G*/, UINT maxSize = 2147483648/*最大尺寸2G*/, const std::set<PathString>* ExcludeSet = NULL,
	bool bDelSrc = false/*是否删除源文件，暂未实现*/,
	PFNMergeDirInfoProc pfn = NULL)
{
	if ((DestPackageFolder == NULL && DestPatchFolder == NULL) || (SrcFolderName == NULL))
	{
		OutputInfo(pfn, "PackResource : source is null or you do want to generate nothing, quit\n");
	}
	else
	{
		OutputInfo(pfn, "PackResource : %s\n", SrcFolderName);
	}

	// 1. generate the old map for referring
	std::map<PathString, MPQPath> RefFilesMap;

	if (RefFolderName != NULL)
	{
		OutputInfo(pfn, "generate ref-file mapping\n");
		GenerateFileToMPQMap(RefFolderName, RefFilesMap, true);
	}

	// don't end with / and '\\'
	check(SrcFolderName && *SrcFolderName != 0);
	check((*(SrcFolderName+strlen(SrcFolderName)-1)!=TEXT('/'))&&(*(SrcFolderName+strlen(SrcFolderName)-1)!=TEXT('\\')));
	check(expectedSize < maxSize);


	// 2. list the new files
	OutputInfo(pfn, "parse files\n", 0);

	std::vector<PathString> SrcFiles;
	EnumFilesInDirectory(SrcFiles, SrcFolderName, NULL, true, ExcludeSet);

	// 3. refine the files new coming
	OutputInfo(pfn, "compare files\n");

	std::vector<PakFileInfo> UnchangedFiles;
	std::vector<PakFileInfo> ChangedFiles;
	std::vector<PakFileInfo> AddedFiles;

	std::vector<PakFileInfo> PakFiles;

	DeleteFileMap DeleteFiles;
	RenameFileMap RenameFiles;

	UINT nFileNameKeyOffset = strlen(SrcFolderName) + 1;

	for (UINT i = 0; i < SrcFiles.size(); ++i)
	{
		const PathString& FileName = SrcFiles[i];
		const char* pFixedFileNameKey = FileName.c_str() + nFileNameKeyOffset;

		std::map<PathString, MPQPath>::iterator itor = RefFilesMap.find(pFixedFileNameKey);

		if (itor != RefFilesMap.end())
		{
			//refine mpq name
			const MPQPath& RefMPQPath = itor->second;

			bool bChanged = (DestPatchFolder && CompareFile(FileName, RefMPQPath.MPQFilePath, RefMPQPath.InMPQPath));

			if (bChanged)
			{
				ChangedFiles.push_back(PakFileInfo(FileName, RefMPQPath.MPQFilePath, RefMPQPath.InMPQPath));
			}
			else
			{
				UnchangedFiles.push_back(PakFileInfo(FileName, RefMPQPath.MPQFilePath, RefMPQPath.InMPQPath));
			}

			RefFilesMap.erase(itor);
		}
		else
		{
			AddedFiles.push_back(FileName);
		}
	}

	for (std::map<PathString, MPQPath>::iterator itor = RefFilesMap.begin(); 
		itor != RefFilesMap.end(); ++itor)
	{
		const PathString& rKey = itor->first;
		MPQPath& rPath = itor->second;

		PushDeleteFile(DeleteFiles, rPath);
		// delete file
	}

	// 4. generate the mpq & patch(if necessary)
	OutputInfo(pfn, "listing packages\n");

	std::map<PathString, ArchiveInfo>	ArchiveMap;

	std::vector<PakFileInfo>* PakFilesArray[3] = {&UnchangedFiles, &ChangedFiles, &AddedFiles};

	for (UINT t = 0; t < 3; ++t)
	{
		std::vector<PakFileInfo>& PakFiles = *(PakFilesArray[t]);

		for (UINT f  = 0; f < PakFiles.size(); ++f)
		{
			const PakFileInfo& InfoPakFile = PakFiles[f];

			check(strstr(InfoPakFile.SrcFileName.c_str(), PAK_EXT_WITH_POINT) == 0);	// mpq contain mpq, forbidden

			PathString strMPQFileBaseName;
			PathString strInMPQPath;

			//FileInfo.RefMPQFileName.Len() > 0
			//&& appStrstr(*FileInfo.RefMPQFileName, FolderName) != NULL 
			//&& *((*pMPQPath->PathMPQ) + appStrlen(FolderName)) != TEXT('\\')
			//&& *((*pMPQPath->PathMPQ) + appStrlen(FolderName)) != TEXT('/')

			strInMPQPath.assign(InfoPakFile.SrcFileName.c_str() + strlen(SrcFolderName) + 1);

			if (t == 0 || t == 1)	// exist file, first use the suggested mpq name
				ExtractPackageFileBaseName(InfoPakFile.RefMPQFilePath, strMPQFileBaseName);

			UINT nIndex = 0;
			UINT nFileSize = (UINT)(GetFileSize(InfoPakFile.SrcFileName.c_str()));

			while (1)
			{
				if (strMPQFileBaseName.size() > 0)
				{
					std::map<PathString, ArchiveInfo>::iterator itor = ArchiveMap.find(strMPQFileBaseName);
					ArchiveInfo* pMPQInfo = ((itor == ArchiveMap.end()) ? NULL : &(itor->second));

					if (pMPQInfo == NULL)
					{
						// new package, must big enough to hold the file
						pMPQInfo = &(ArchiveMap[strMPQFileBaseName]);
					}

					check(pMPQInfo);

					UINT nEvaluatedSize = pMPQInfo->ArchiveRawSize + nFileSize;
					UINT nLimitedSize = (nIndex == 0 ? maxSize : expectedSize);

					if (nEvaluatedSize <= nLimitedSize )
					{
						// store it
						pMPQInfo->AddFile(InfoPakFile.SrcFileName.c_str(), strInMPQPath.c_str(), nFileSize);

						//need pack it imm? and then we know the exact size
						//MPQPath; FileName; strPathInMPQ;

						if (DestPatchFolder != NULL)
						{
							// we wanna generate patch
							switch (t)
							{
							case 0:	// unchanged files
								if (nIndex == 0)
								{
									// same mpq, diff sub path, rename
									if (strInMPQPath != InfoPakFile.RefInMPQPath)
									{
										PushRenameFile(RenameFiles, MPQPath(InfoPakFile.RefMPQFilePath, InfoPakFile.RefInMPQPath), strInMPQPath);
									}
									else
									{
										// same mpq, same sub path, do nothing
									}
								}
								else
								{
									// diff mpq, delete old, add to patch

									PushDeleteFile(DeleteFiles, MPQPath(InfoPakFile.RefMPQFilePath, InfoPakFile.RefInMPQPath));
									pMPQInfo->AddPatchFile(InfoPakFile.SrcFileName.c_str(), strInMPQPath.c_str());
								}
								break;

							case 1:	// changed files
								if (nIndex  == 0 && strInMPQPath == InfoPakFile.RefInMPQPath)
								{
									// same mpq, same sub path, patch
									pMPQInfo->AddPatchFile(InfoPakFile.SrcFileName.c_str(), strInMPQPath.c_str());
								}
								else
								{
									// diff mpq, or diff sub-path, delete old, patch
									PushDeleteFile(DeleteFiles, MPQPath(InfoPakFile.RefMPQFilePath, InfoPakFile.RefInMPQPath));
									pMPQInfo->AddPatchFile(InfoPakFile.SrcFileName.c_str(), strInMPQPath.c_str());
								}
								break;

							case 2:	// new files
								pMPQInfo->AddPatchFile(InfoPakFile.SrcFileName.c_str(), strInMPQPath.c_str());
								break;

							default:
								check(0);
								break;
							}
						}

						break;
					}
					else
					{
						// file is too large to pack
						check(nFileSize <= expectedSize);
					}
				}

				char szTemp[64];
				sprintf_s(szTemp, sizeof(szTemp), PAK_BASENAME_FORMAT, nIndex);
				strMPQFileBaseName = szTemp;

				++nIndex;
			}
		}
	}

	// 5. build data, write mpqs
	const float FileCountCoefficient = 2;
	OutputInfo(pfn, "begin write packages...\n");

	if (DestPackageFolder)
	{
		CreateDirectoryHierarchy(DestPackageFolder, true);
	}

	if (DestPatchFolder)
	{
		CreateDirectoryHierarchy(DestPatchFolder, true);
	}

	for (std::map<PathString, ArchiveInfo>::iterator itor = ArchiveMap.begin(); 
		itor != ArchiveMap.end(); ++itor)
	{
		const PathString& strArchiveBaseName = itor->first;
		const ArchiveInfo& rArchiveInfo = itor->second;

		check(rArchiveInfo.EntireFiles.size() == rArchiveInfo.EntirePathsInMPQ.size());
		check(rArchiveInfo.PatchFiles.size() == rArchiveInfo.PatchPathsInMPQ.size());

		PathString strArchivePath[2];

		if (DestPackageFolder)
		{
			strArchivePath[0].assign(DestPackageFolder); 
			strArchivePath[0] += "\\";
			strArchivePath[0] += strArchiveBaseName;
			strArchivePath[0] += PAK_EXT_WITH_POINT;
		}

		if (DestPatchFolder)
		{
			strArchivePath[1].assign(DestPatchFolder); 
			strArchivePath[1] += "\\";
			strArchivePath[1] += strArchiveBaseName;

			if (PatchPostfix)
				strArchivePath[1] += PatchPostfix;

			strArchivePath[1] += PAK_EXT_WITH_POINT;
		}

		for (int a = 0; a < 2; ++a)
		{
			// 0 is for entire package, 1 is for the patch package

			if (strArchivePath[a].size() > 0 && rArchiveInfo.Files[a].size() > 0)
			{
				HANDLE hArchive = NULL;
				
				SFileCreateArchive(strArchivePath[a], 0, (DWORD)(rArchiveInfo.Files[a].size() * FileCountCoefficient), &hArchive);
				//rArchiveInfo.hArchive = hArchive;

				OutputInfo(pfn, "write package [%s] ... ", strArchivePath[a].c_str());

				if (hArchive)
				{
					for (UINT i = 0; i < rArchiveInfo.Files[a].size(); ++i)
					{
						const PathString& SrcFilePath = rArchiveInfo.Files[a][i];
						const PathString& DestPathInMPQ = rArchiveInfo.PathsInMPQ[a][i];

						SFileAddFileEx(hArchive, SrcFilePath.c_str(), DestPathInMPQ.c_str(), PAK_CREATE_FLAG, PAK_COMPRESSION, PAK_COMPRESSION_NEXT);
					}

					SFileCloseArchive(hArchive);

					OutputInfo(pfn, "succeed\n");
				}
				else
				{
					int nErrorCode = GetLastError();

					switch (nErrorCode)
					{
					case 183:
						// already exist
						OutputInfo(pfn, "failed, already exist\n");
						break;
					}
				}
			}
		}
	}

	OutputInfo(pfn, "end write packages!\n");

	// 6. write the rename & delete

	if (DestPatchFolder && DeleteFiles.size() > 0 || RenameFiles.size() > 0)
	{
		OutputInfo(pfn, "write rename-del info\n");
		PathString DestDelRenPak(DestPatchFolder);
		DestDelRenPak += "\\";
		DestDelRenPak += PATCH_DEL_RENAME_TABLE_PAK;

		HANDLE hDelRenArchive = NULL;

		if (SFileCreateArchive(DestDelRenPak, 0, 8, &hDelRenArchive))
		{
			//////////////////////////////////////////////////////////////////////////
			PathString data;

			if (DeleteFiles.ToString(data))
			{
				HANDLE hDelTable = NULL;

				if (SFileCreateFile(hDelRenArchive, PATCH_DEL_TABLE_FILE, 0, data.size(), 0, PAK_CREATE_FLAG, &hDelTable))
				{
					SFileWriteFile(hDelTable, data.c_str(), data.size(), PAK_COMPRESSION);

					SFileFinishFile(hDelTable);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			data.clear();

			if (RenameFiles.ToString(data))
			{
				HANDLE hRenTable = NULL;

				if (SFileCreateFile(hDelRenArchive, PATCH_RENAME_TABLE_FILE, 0, data.size(), 0, PAK_CREATE_FLAG, &hRenTable))
				{
					SFileWriteFile(hRenTable, data.c_str(), data.size(), PAK_COMPRESSION);

					SFileFinishFile(hRenTable);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			SFileCloseArchive(hDelRenArchive);
		}
	}


	// 6. files need to be delete
	if (bDelSrc)
	{
		check(strstr(DestPackageFolder, SrcFolderName) == NULL);
		check(DestPatchFolder == NULL || strstr(DestPatchFolder, SrcFolderName) == NULL);
	}

	OutputInfo(pfn, "packages done\n");

	return (ArchiveMap.size() > 0);

	return 0;
}

bool EnsureArchiveCapacity(HANDLE archive, DWORD additionalFiles)
{
	DWORD files = 0, maximumFiles = 0;
	if (!SFileGetFileInfo(archive, SFileMpqNumberOfFiles, &files, sizeof(files), NULL) ||
		!SFileGetFileInfo(archive, SFileMpqMaxFileCount, &maximumFiles, sizeof(maximumFiles), NULL) ||
		additionalFiles > (DWORD)-1 - files)
		return false;
	const DWORD required = files + additionalFiles;
	return required < maximumFiles || SFileSetMaxFileCount(archive, required);
}

template<class Table>
bool LoadPatchTable(HANDLE file, char* localBuffer, DWORD localBufferSize, Table& table)
{
	DWORD sizeHigh = 0;
	const DWORD size = SFileGetFileSize(file, &sizeHigh);
	if (sizeHigh != 0 || size == (DWORD)-1)
		return false;
	std::unique_ptr<char[]> storage;
	if (size > localBufferSize)
		storage.reset(new (std::nothrow) char[size]);
	char* buffer = size <= localBufferSize ? localBuffer : storage.get();
	if (!buffer)
		return false;
	DWORD read = 0;
	const bool complete = SFileReadFile(file, buffer, size, &read, NULL) && read == size;
	bool valid = false;
	if (complete)
		valid = table.FromString(PathString(buffer, size));
	return valid;
}

unsigned int ApplyPatch(const char* DestFolder, const char* PatchFolder, bool CompactDestArchive = false, PFNMergeDirInfoProc pfn = NULL)
{
	unsigned int nPatchedFile = 0;

	check(DestFolder && PatchFolder);

	const unsigned int LocalBufferSize = 16*1024*1024;	// 16M buffer
	char* LocalBuffer = new char[LocalBufferSize];

	DeleteFileMap DeleFiles;
	RenameFileMap RenameFiles;

	PathString DelRenArchiveName(PatchFolder);
	DelRenArchiveName += "\\";
	DelRenArchiveName += PATCH_DEL_RENAME_TABLE_PAK;

	HANDLE hDelRenArchive = NULL; 

	// construct del-ren map
	if (SFileOpenArchive(DelRenArchiveName, 0, STREAM_PROVIDER_FLAT, &hDelRenArchive))
	{
		HANDLE hFile = NULL;
		if (SFileOpenFileEx(hDelRenArchive, PATCH_DEL_TABLE_FILE, 0, &hFile))
		{
			const bool valid = LoadPatchTable(hFile, LocalBuffer, LocalBufferSize, DeleFiles);
			SFileCloseFile(hFile);
			if (!valid)
			{
				SFileCloseArchive(hDelRenArchive);
				delete[] LocalBuffer;
				return 0;
			}
		}

		if (SFileOpenFileEx(hDelRenArchive, PATCH_RENAME_TABLE_FILE, 0, &hFile))
		{
			const bool valid = LoadPatchTable(hFile, LocalBuffer, LocalBufferSize, RenameFiles);
			SFileCloseFile(hFile);
			if (!valid)
			{
				SFileCloseArchive(hDelRenArchive);
				delete[] LocalBuffer;
				return 0;
			}
		}

		SFileCloseArchive(hDelRenArchive);
	}

	// patch
	std::vector<PathString> DestFiles;
	EnumFilesInDirectory(DestFiles, DestFolder, PAK_EXT_WITH_POINT, false);

	std::vector<PathString> PatchFiles;
	EnumFilesInDirectory(PatchFiles, PatchFolder, PAK_EXT_WITH_POINT, false);

	unsigned int nPatchFileNameOffset = strlen(PatchFolder) + 1;
	unsigned int nDestFileNameOffset = strlen(DestFolder) + 1;

	for (unsigned int d = 0; d < DestFiles.size(); ++d)
	{
		const PathString& DestFile = DestFiles[d];

		const char* pNakedDestName = DestFile.c_str() + nDestFileNameOffset;

		DeleteFileMap::iterator itorD = DeleFiles.find(pNakedDestName);

		RenameFileMap::iterator itorR = RenameFiles.find(pNakedDestName);

		int PatchFileIndex = -1;

		for (unsigned int p = 0; p < PatchFiles.size(); ++p)
		{
			const PathString& PatchFile = PatchFiles[p];

			const char* pNakedPatchName = PatchFile.c_str() + nPatchFileNameOffset;

			unsigned int b = 0;
			for (; b < PAK_BASENAME_LEN; ++b)
			{
				if (*(pNakedPatchName+b) != *(pNakedDestName+b))
				{
					break;
				}
			}

			if (b == PAK_BASENAME_LEN)
			{
				// found
				PatchFileIndex = p;
				break;
			}
		}

		bool bNeedOpenArchive = (itorD != DeleFiles.end() || itorR != RenameFiles.end() || PatchFileIndex != -1);
		HANDLE hDestArchive = NULL;
		bool bNeedCompact = false;

		if (bNeedOpenArchive && SFileOpenArchive(DestFile, 0, STREAM_PROVIDER_FLAT, &hDestArchive))
		{
			check(hDestArchive);

			// 1. delete
			if (itorD != DeleFiles.end())
			{
				OutputInfo(pfn, "%s delete sub files", pNakedDestName);

				const PathString& Pak = itorD->first;
				const std::vector<PathString>& SubPaths = itorD->second;

				for (unsigned int p = 0; p < SubPaths.size(); ++p)
				{
					const PathString& DelFileName = SubPaths[p];

					SFileRemoveFile(hDestArchive, DelFileName, 0);

					bNeedCompact = true;
				}

				nPatchedFile += SubPaths.size();
			}

			// 2. rename
			if (itorR != RenameFiles.end())
			{
				OutputInfo(pfn, "%s rename sub files", pNakedDestName);

				const PathString& Pak = itorR->first;
				const std::vector<std::pair<PathString,PathString>>& SubPaths = itorR->second;

				for (unsigned int p = 0; p < SubPaths.size(); ++p)
				{
					const PathString& OldFileName = SubPaths[p].first;
					const PathString& NewFileName = SubPaths[p].second;
					SFileRenameFile(hDestArchive, OldFileName, NewFileName);
				}

				nPatchedFile += SubPaths.size();
			}

			// 3. patch
			if (PatchFileIndex != -1)
			{
				HANDLE hPatchArchive = NULL;
				const PathString& PatchFile = PatchFiles[PatchFileIndex];

				if (SFileOpenArchive(PatchFile, 0, STREAM_PROVIDER_FLAT | STREAM_FLAG_READ_ONLY, &hPatchArchive))
				{
					check(hPatchArchive);

					DWORD NumPatchFiles = 0;
					if (!SFileGetFileInfo(hPatchArchive, SFileMpqNumberOfFiles, &NumPatchFiles, sizeof(NumPatchFiles), NULL) ||
						!EnsureArchiveCapacity(hDestArchive, NumPatchFiles))
					{
						SFileCloseArchive(hPatchArchive);
						SFileCloseArchive(hDestArchive);
						delete[] LocalBuffer;
						return nPatchedFile;
					}

					SFILE_FIND_DATA fd;
					HANDLE hFind = SFileFindFirstFile(hPatchArchive, "*", &fd, NULL);

					DWORD NumPatchedFiles = 0;

					if (hFind)
					{
						do 
						{
							++NumPatchedFiles;
							OutputInfo(pfn, "%s patch sub files %d/%d", pNakedDestName, NumPatchedFiles, NumPatchFiles);

							if (strcmp(fd.cFileName, "(listfile)") != 0 && strcmp(fd.cFileName, "(attributes)") != 0)
							{
								HANDLE hPatchFile = NULL;
								HANDLE hDestFile = NULL;

								if (SFileOpenFileEx(hPatchArchive, fd.cFileName, 0, &hPatchFile))
								{
									DWORD FileSize = 0, FileSizeHigh = 0;

									FileSize = SFileGetFileSize(hPatchFile, &FileSizeHigh);

									if (FileSizeHigh != 0 || FileSize == (DWORD)-1)
									{
										SFileCloseFile(hPatchFile);
										continue;
									}


									char* Buffer = ((LocalBufferSize >= FileSize) ? (LocalBuffer) : (new char[FileSize]));

									DWORD dwRead = 0;

									if (SFileReadFile(hPatchFile, Buffer, FileSize, &dwRead, NULL) && dwRead == FileSize &&
										SFileCreateFile(hDestArchive, fd.cFileName, 0, FileSize, fd.lcLocale, PAK_CREATE_FLAG, &hDestFile))
									{
										check(FileSize == dwRead);
										SFileWriteFile(hDestFile, Buffer, dwRead, PAK_COMPRESSION);
										SFileFinishFile(hDestFile);

										bNeedCompact = true;
									}

									if (Buffer != LocalBuffer)
										delete [] Buffer;

									SFileCloseFile(hPatchFile);
								}
							}

						} while (SFileFindNextFile(hFind, &fd));

						SFileFindClose(hFind);

						nPatchedFile += NumPatchFiles;
					}

					SFileCloseArchive(hPatchArchive);
				}
			}

			if (bNeedCompact && CompactDestArchive)
			{
				OutputInfo(pfn, "%s Compact", pNakedDestName);
				SFileCompactArchive(hDestArchive, NULL, 0);
			}

			SFileCloseArchive(hDestArchive);
		}
	}

	delete [] LocalBuffer;

	return nPatchedFile;
}

void CompactFolder(const char* DestFolder)
{
	std::vector<PathString> Files;
	EnumFilesInDirectory(Files, DestFolder, PAK_EXT_WITH_POINT, true);

	for (unsigned int i = 0; i < Files.size(); ++i)
	{
		const PathString& FilePath = Files[i];

		HANDLE hArchive;

		if (SFileOpenArchive(FilePath, 0, STREAM_PROVIDER_FLAT, &hArchive))
		{
			SFileCompactArchive(hArchive, NULL, 0);

			SFileCloseArchive(hArchive);
		}
	}
}

//void DelFileInternal()
//{
//
//}
//
//void AddFileInternal()
//{
//
//}

bool MergeFileInternal(const char* pSrcFileDir, const char* pDeltaFileDir, const char* pSubFileDir, const char* pFileNakedName, PFNMergeDirInfoProc pfn)
{
	PathString strSrcFile(pSrcFileDir);
	strSrcFile += PathString(pFileNakedName);
	PathString strDeltaFile(pDeltaFileDir);
	strDeltaFile += PathString(pFileNakedName);

	const char* pSubDirWithoutFirstSlash = NULL;
	unsigned int nSubDirLen = 0;

	if ((*pSubFileDir) != 0)
	{
		pSubDirWithoutFirstSlash = pSubFileDir + 1;
		nSubDirLen = strlen(pSubDirWithoutFirstSlash) + 1;
	}

	PathString strSubFile(pFileNakedName);
	if (pSubDirWithoutFirstSlash)
	{
		strSubFile = PathString(pSubDirWithoutFirstSlash) + PathString(pFileNakedName);
		int deltaIndex = strSubFile.find(".delta");
		if (deltaIndex != -1)
		{
			strSubFile = strSubFile.substr(0, deltaIndex);
		}
	}

	PathString strFileDir = strSrcFile.substr(0, strSrcFile.find_last_of('\\') + 1);
	if (!IsDirExist(strFileDir))
	{
		MakeSureDirectoryPathExists(strFileDir);
	}

	WCHAR szSrcDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strSrcFile.c_str(), -1, szSrcDir, (int)(sizeof(szSrcDir) / sizeof(szSrcDir[0]))))
		return false;

	WCHAR szDeltaDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strDeltaFile.c_str(), -1, szDeltaDir, (int)(sizeof(szDeltaDir) / sizeof(szDeltaDir[0]))))
		return false;

	bool bSucc = false;
	int index = strSrcFile.find(".delta");
	if (index != -1)
	{// 合并差异文件
		strSrcFile = strSrcFile.substr(0, index);
		if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strSrcFile.c_str(), -1, szSrcDir, (int)(sizeof(szSrcDir) / sizeof(szSrcDir[0]))))
			return false;

		ResultMD5List;
		std::map<std::string, std::string>::iterator iterRes;
		PathString strMD5("0");
		if (pResultMD5List)
		{
			iterRes = pResultMD5List->find(strSubFile);
			if (iterRes != pResultMD5List->end())
			{
				strMD5 = iterRes->second;
			}
		}

		if (CompareFileWithMD5(strSrcFile, strMD5))
		{
			// 如果已经合并过了
			PathString text("merge file:");
			text += strSrcFile;
			text += PathString(" success！\n");
			OutputInfo(pfn, "%s", text.c_str());
			bSucc = true;
		}
		else
		{
			bSucc = (*pfMF)(szSrcDir, szDeltaDir, szSrcDir);
			if (bSucc)
			{
				PathString text("merge file:");
				text += strSrcFile;
				text += PathString(" success！\n");
				OutputInfo(pfn, "%s", text.c_str());
			}
			else
			{
				PathString text("merge file:");
				text += strSrcFile;
				text += PathString(" failed！\n");
				OutputInfo(pfn, "%s", text.c_str());
			}
		}
	}
	else
	{// 拷贝新增文件
		if (IsFileExist(strSrcFile))
		{// 去除文件的只读属性， 否则会拷贝失败
			DWORD attr = ::GetFileAttributes((LPCTSTR)szSrcDir);
			attr &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes((LPCTSTR)szSrcDir, attr);
		}

		bSucc = CopyFile(strDeltaFile, strSrcFile, FALSE) ? true : false;
		if (bSucc)
		{
			PathString text("copy file:");
			text += strSrcFile;
			text += PathString(" success！\n");
			OutputInfo(pfn, "%s", text.c_str());
		}
		else
		{
			PathString text("copy file:");
			text += strSrcFile;
			text += PathString(" failed！\n");
			OutputInfo(pfn, "%s", text.c_str());
		}
	}

	return bSucc;
}

bool IsMPQFolder(const char* FolderName)
{
	if (FolderName)
	{
		bool bCookedPC = (strstr(FolderName, "MMOGame") != NULL);

		return bCookedPC;
	}
	else
	{
		return false;
	}
}

enum eConcern
{
	E_CONCERN,
	E_NOT_CONCERN,
	E_NOT_SURE,
};

eConcern IsMyConcern(const char* pSubDir)
{
	NonePakSubDirs;
	PakSubDirs;

	// upper and lower case
	for (unsigned int i = 0; i < PakSubDirs.size(); ++i)
	{
		const PathString& PakSubDir = PakSubDirs[i];

		if (strcmp(PakSubDir, pSubDir) == 0)
		{
			return E_CONCERN;
		}
		
		if (strstr(PakSubDir, pSubDir) != NULL)
		{
			return E_NOT_SURE;
		}
	}
	
	return E_NOT_CONCERN;
}

bool ExtractFromPak2Folder(const std::vector<PathString>& vecPakFiles, const PathString& strLocalPath, int& PakIndex)
{
	CreateDirectoryHierarchy(strLocalPath, false);

	bool bSucc = false;

	PakIndex = -1;

	for (unsigned int i = 0; i < vecPakFiles.size() && (PakIndex == -1); ++i)
	{
		const PathString& strPak = vecPakFiles[i];

		HANDLE hArchive = NULL;

		if (SFileOpenArchive(strPak, 0, STREAM_PROVIDER_FLAT | STREAM_FLAG_READ_ONLY, &hArchive))
		{
			HANDLE hFile = NULL;

			if (SFileOpenFileEx(hArchive, strLocalPath, 0, &hFile))
			{
				PakIndex = i;	// find it

				DWORD FileSize = 0, FileSizeHigh = 0;

				FileSize = SFileGetFileSize(hFile, &FileSizeHigh);

				if (FileSizeHigh != 0 || FileSize == (DWORD)-1)
				{
					SFileCloseFile(hFile);
					SFileCloseArchive(hArchive);
					return false;
				}

				char* Buffer = new char[FileSize];

				DWORD dwRead = 0;

				if (SFileReadFile(hFile, Buffer, FileSize, &dwRead, NULL) && dwRead == FileSize)
				{
					check(FileSize == dwRead);
				
					FILE* pFile = NULL;

					fopen_s(&pFile, strLocalPath, ("wb"));

					if (pFile)
					{
						bSucc = (fwrite(Buffer, 1, FileSize, pFile) == FileSize);
						fclose(pFile);
					}
				}

				if (Buffer)
					delete [] Buffer;

				SFileCloseFile(hFile);
			}

			SFileCloseArchive(hArchive);
		}
	}

	return bSucc;
}


#include <algorithm>

bool MergeDirPakInternal(const char* pSrcDir, const char* pDeltaDir, const char* pSubDir, PFNMergeDirInfoProc pfn)
{
	check(pSrcDir && pDeltaDir && pSubDir);

	PathString strSrcDir(pSrcDir);
	strSrcDir += pSubDir;

	PathString strDeltaDir(pDeltaDir);
	strDeltaDir += pSubDir;

	const char* pSubDirWithoutFirstSlash = NULL;
	unsigned int nSubDirLen = 0;
	
	if ((*pSubDir) != 0)
	{
		pSubDirWithoutFirstSlash = pSubDir + 1;
		nSubDirLen = strlen(pSubDirWithoutFirstSlash) + 1;
	}

	//////////////////////// 1. listing delta file ////////////////////////
	std::vector<PathString> vecDeltaFiles;
	EnumFilesInDirectory(vecDeltaFiles, strDeltaDir);

	std::vector<PathString> vecPackFiles;
	EnumFilesInDirectory(vecPackFiles, strSrcDir, PAK_EXT_WITH_POINT);

	//////////////////////// 2. delete files if have ////////////////////////

	DelFileList;
	std::map<std::string, std::string>::iterator iterDel;
	ResultMD5List;
	std::map<std::string, std::string>::iterator iterRes;

	OutputInfo(pfn, "parse files\n");


	// 2.1 delete in windows, it is not my business
	//for (itor = pDelFileList->begin(); itor != pDelFileList->end(); ++itor)
	//{
	//	const std::string& DelFileName = itor->first;

	//	DeleteFile(DelFileName.c_str());
	//}

	// 2.2 delete in pak
	for (unsigned int i = 0; i < vecPackFiles.size(); ++i)
	{
		const PathString& strPak = vecPackFiles[i];

		HANDLE hArchive = NULL;

		if (SFileOpenArchive(strPak, 0, STREAM_PROVIDER_FLAT, &hArchive))
		{
			check(hArchive);

			if (pDelFileList)
			{
				for (iterDel = pDelFileList->begin(); iterDel != pDelFileList->end();)
				{
					const std::string& filename = iterDel->first;

					if (pSubDirWithoutFirstSlash && strstr(filename.c_str(), pSubDirWithoutFirstSlash) != NULL)
					{
						const char* pArchivedName = filename.c_str() + nSubDirLen;
						bool bSucc = SFileRemoveFile(hArchive, pArchivedName, 0);

						if (bSucc)
						{
							iterDel = pDelFileList->erase(iterDel);
							continue;
						}
					}

					++iterDel;
				}
			}

			SFileCloseArchive(hArchive);
		}
	}
	
	//////////////////////// 3. extract the src file from mpq ////////////////////////
	struct PakFileIndex
	{
		int iPakIndex;
		unsigned int iFileIndex;

		PakFileIndex()
			: iPakIndex(-1), iFileIndex(0)
		{
		}

		PakFileIndex(int p, unsigned int f)
		{
			iPakIndex = p;
			iFileIndex = f;
		}

		bool operator < (const PakFileIndex& rhs) const
		{
			return iPakIndex < rhs.iPakIndex;
		}
	};

	OutputInfo(pfn, "extract files\n");

	std::vector<PakFileIndex> vecPakFileIndex(vecDeltaFiles.size());

	unsigned int nDeltaDirLen = strDeltaDir.length();

	PathString strSrcTempDir(strSrcDir);
	strSrcTempDir += "\\Temp";
	CreateDirectoryHierarchy(strSrcTempDir, true);

	ClientPatch::ScopedCurrentDirectory<char> currentDirectory;
	if (!currentDirectory.Change(strSrcTempDir))
		return false;

	//////////////////////// 4. merge dir ////////////////////////

	WCHAR Src[MAX_PATH];
	WCHAR Delta[MAX_PATH];

	if (!ClientPatch::ToWide(strSrcTempDir, Src))
		return false;
	if (!ClientPatch::ToWide(strDeltaDir, Delta))
		return false;

	if (!(*pfMD)(Src, Delta, Src, pfn))
	{
		OutputInfo(pfn, "merge dir failed\n");
		return false;
	}
	
	bool bSucc = true;
	for (unsigned int i = 0; i < vecDeltaFiles.size(); ++i)
	{
		PathString& strDeltaFilePath = vecDeltaFiles[i];

		PathString strFilePath;
		int index = strDeltaFilePath.find(".delta");
		if (index != -1)
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - std::string(".delta").length() - 1);
		}
		else
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - 1);
		}

		PathString strTempFilePath = strSrcTempDir + "\\" + strFilePath;
		PathString strTempFileDir = strTempFilePath.substr(0, strTempFilePath.find_last_of('\\') + 1);
		if (!IsDirExist(strTempFileDir))
		{
			MakeSureDirectoryPathExists(strTempFileDir);
		}
		
		WCHAR szDeltaFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strDeltaFilePath.c_str(), szDeltaFilePath))
			return false;

		WCHAR szTempFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strTempFilePath.c_str(), szTempFilePath))
			return false;

		int iPakIndex = -1;

		if (index != -1)
		{
			bSucc = ExtractFromPak2Folder(vecPackFiles, strFilePath, iPakIndex);
			if (bSucc)
			{
				PathString text("extract file:");
				text += strFilePath;
				text += PathString(" success！\n");
				OutputInfo(pfn, "%s", text.c_str());

				PathString strSubFile = strFilePath;
				if (pSubDirWithoutFirstSlash)
				{
					strSubFile = PathString(pSubDirWithoutFirstSlash) + "\\" + strFilePath;
				}
				PathString strMD5("0");
				if (pResultMD5List)
				{
					iterRes = pResultMD5List->find(strSubFile);
					if (iterRes != pResultMD5List->end())
					{
						strMD5 = iterRes->second;
					}
				}

				if (CompareFileWithMD5(strTempFilePath, strMD5))
				{
					// 如果已经合并过了
					bSucc = true;
					text = PathString("merge file:");
					text += strTempFilePath;
					text += PathString(" success！\n");
					OutputInfo(pfn, "%s", text.c_str());
				}
				else
				{
					bSucc = (*pfMF)(szTempFilePath, szDeltaFilePath, szTempFilePath);
					if (bSucc)
					{
						text = PathString("merge file:");
						text += strTempFilePath;
						text += PathString(" success！\n");
						OutputInfo(pfn, "%s", text.c_str());
					}
					else
					{
						text = PathString("merge file:");
						text += strTempFilePath;
						text += PathString(" failed！\n");
						OutputInfo(pfn, "%s", text.c_str());
						return false;
					}
				}
			}
			else
			{
				PathString text("extract file:");
				text += strTempFilePath;
				text += PathString(" failed！\n");
				OutputInfo(pfn, "%s", text.c_str());
			}
		}
		else
		{
			if (IsFileExist(strTempFilePath))
			{// 去除文件的只读属性， 否则会拷贝失败
				DWORD attr = ::GetFileAttributes((LPCTSTR)szTempFilePath);
				attr &= ~FILE_ATTRIBUTE_READONLY;
				::SetFileAttributes((LPCTSTR)szTempFilePath, attr);
			}

			bSucc = CopyFile(strDeltaFilePath, strTempFilePath, FALSE) ? true : false;
			if (bSucc)
			{
				PathString text("copy file:");
				text += strTempFilePath;
				text += PathString(" success！\n");
				OutputInfo(pfn, "%s", text.c_str());
			}
			else
			{
				PathString text("copy file:");
				text += strTempFilePath;
				text += PathString(" failed！\n");
				OutputInfo(pfn, "%s", text.c_str());
				return false;
			}
		}

		vecPakFileIndex[i] = PakFileIndex(iPakIndex, i);
	}

	//////////////////////// 5. add all file into mpq(include added) ////////////////////////

	std::sort(vecPakFileIndex.begin(), vecPakFileIndex.end());

	INT iPakIndex = 0x7fffffff;

	HANDLE hArchive = NULL;

	for (unsigned int i = 0; i < vecPakFileIndex.size(); ++i)
	{
		OutputInfo(pfn, "merge files %d/%d\n", i, vecPakFileIndex.size());

		const PakFileIndex& fileIndex = vecPakFileIndex[i];
		const PathString& strDeltaFilePath = vecDeltaFiles[fileIndex.iFileIndex];

		PathString strFilePath;
		int index = strDeltaFilePath.find(".delta");
		if (index != -1)
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - std::string(".delta").length() - 1);
		}
		else
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - 1);
		}

		if (iPakIndex != fileIndex.iPakIndex)
		{
			// open archive
			iPakIndex = fileIndex.iPakIndex;

			if (hArchive)
			{
				SFileCloseArchive(hArchive);
				hArchive = NULL;
			}

			if (iPakIndex == -1)
			{
				// attach to small mpq, or create new archive

				DWORD dwNewFileCount = (DWORD)(vecPakFileIndex.size() - i);
				if (vecPackFiles.size() > 0)
				{
					if (GetFileSize(vecPackFiles.back()) > PAK_SIZE_THRESHOLD)
					{
						char szPakName[MAX_PATH];
						sprintf_s(szPakName, sizeof(szPakName), PAK_FULLNAME_FORMAT, vecPackFiles.size());

						vecPackFiles.push_back(szPakName);

						SFileCreateArchive(szPakName, 0, dwNewFileCount, &hArchive);
					}
					else
					{
						SFileOpenArchive(vecPackFiles.back(), 0, STREAM_PROVIDER_FLAT, &hArchive);
						if (hArchive && !EnsureArchiveCapacity(hArchive, dwNewFileCount))
						{
							SFileCloseArchive(hArchive);
							hArchive = NULL;
						}
					}
				}
				else
				{
					char szPakName[MAX_PATH];
					sprintf_s(szPakName, sizeof(szPakName), PAK_FULLNAME_FORMAT, vecPackFiles.size());

					vecPackFiles.push_back(szPakName);

					SFileCreateArchive(szPakName, 0, dwNewFileCount, &hArchive);
				}
			}
			else
			{
				SFileOpenArchive(vecPackFiles[iPakIndex].c_str(), 0, STREAM_PROVIDER_FLAT, &hArchive);
			}
		}

		if (!hArchive)
		{
			currentDirectory.Restore();
			return false;
		}

		bSucc = SFileAddFileEx(hArchive, strFilePath.c_str(), strFilePath.c_str(), PAK_CREATE_FLAG, PAK_COMPRESSION, PAK_COMPRESSION_NEXT);
		if (bSucc)
		{
			PathString text("add file:");
			text += strFilePath;
			text += PathString(" success！\n");
			OutputInfo(pfn, "%s", text.c_str());
		}
		else
		{
			if (hArchive)
			{
				SFileCloseArchive(hArchive);
			}

			PathString text("add file:");
			text += strFilePath;
			text += PathString(" failed！\n");
			OutputInfo(pfn, "%s", text.c_str());
			currentDirectory.Restore();
			return false;
		}
	}

	if (hArchive)
	{
		SFileCloseArchive(hArchive);
	}

	//DeleteFileTree(strSrcDirTemp);
	DeleteFilesInDirectory(strSrcTempDir, true);
	PathString text("remove temp file:");
	text += strSrcTempDir;
	text += PathString("\n");
	OutputInfo(pfn, "%s", text.c_str());

	currentDirectory.Restore();

	// 6. shrink mpq if need
	if (0)
	{
		OutputInfo(pfn, "shrink packages\n");
	}
	
	return true;
}

bool MergeDirInternal(const char* pSrcDir, const char* pDeltaDir, const char* pShareSubDir, PFNMergeDirInfoProc pfn)
{
	unsigned int nMergedFile = 0;

	WIN32_FIND_DATA fd;

	PathString strFindDir(pDeltaDir);
	strFindDir += pShareSubDir;
	strFindDir += "\\*.*";

	bool bSucc =  true;

	HANDLE Handle=FindFirstFile(strFindDir, &fd);
	if (Handle!=INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.cFileName[0] != '.')
			{
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					PathString strSubDir(pShareSubDir);
					strSubDir += "\\";
					strSubDir += fd.cFileName;

					eConcern e = IsMyConcern(strSubDir.c_str() + 1);

					switch (e)
					{
					case E_NOT_CONCERN:
					case E_NOT_SURE:
						{
							// throw to xdelta3
							PathString strSrcDir(pSrcDir);
							PathString strDeltaDir(pDeltaDir);

							strSrcDir += strSubDir;
							strDeltaDir += strSubDir;

							WCHAR Src[MAX_PATH];
							WCHAR Delta[MAX_PATH];

							if (!ClientPatch::ToWide(strSrcDir, Src))
							{
								bSucc = false;
								break;
							}
							if (!ClientPatch::ToWide(strDeltaDir, Delta))
							{
								bSucc = false;
								break;
							}

							bSucc = bSucc && ((*pfMD)(Src, Delta, Src, pfn));
							bSucc = bSucc && MergeDirInternal(pSrcDir, pDeltaDir, strSubDir, pfn);
						}
						break;
					case E_CONCERN:
						{
							bSucc = bSucc && MergeDirPakInternal(pSrcDir, pDeltaDir, strSubDir, pfn);
						}
						break;
					}
				}
				else
				{
					PathString strSubDir(pShareSubDir);
					strSubDir += "\\";

					eConcern e = IsMyConcern(strSubDir.c_str() + 1);

					switch (e)
					{
					case E_NOT_CONCERN:
					case E_NOT_SURE:
						{
							PathString strSrcDir(pSrcDir);
							PathString strDeltaDir(pDeltaDir);

							strSrcDir += strSubDir;
							strDeltaDir += strSubDir;
	
							bSucc = bSucc & MergeFileInternal(strSrcDir, strDeltaDir, strSubDir, fd.cFileName, pfn);
						}
						break;
					case E_CONCERN:
						// 如果是包内文件, 交给PatchPack，这里不处理
						break;
					}
				}
			}
		} while (FindNextFile(Handle,&fd) && bSucc);

		FindClose(Handle);
	}

	if (bSucc)
	{
		PathString text("merge dir internal:");
		text += strFindDir;
		text += PathString("success！\n");
		OutputInfo(NULL, "%s", text.c_str());
	}
	else
	{
		PathString text("merge dir internal:");
		text += strFindDir;
		text += PathString("failed！\n");
		OutputInfo(NULL, "%s", text.c_str());
	}

	return bSucc;
}

extern "C" __declspec(dllexport) bool MergeFile(LPCWSTR lpszSrcFilePath, LPCWSTR lpszDeltaFilePath, LPCWSTR lpszDestFilePath)
{
	if (lpszSrcFilePath && lpszDeltaFilePath)
	{
		char szSrcDir[1024];
		char szDeltaDir[1024];

		if (!WideCharToMultiByte(CP_ACP,0,lpszSrcFilePath, -1,szSrcDir,sizeof(szSrcDir),NULL,NULL))
			return 0;
		if (!WideCharToMultiByte(CP_ACP,0,lpszDeltaFilePath, -1,szDeltaDir,sizeof(szDeltaDir),NULL,NULL))
			return 0;

		char szDriver[MAX_PATH];
		char szDir[MAX_PATH];
		char szBase[MAX_PATH];
		char szExt[MAX_PATH];

		// src
		_splitpath_s(szSrcDir, szDriver, sizeof(szDriver), szDir, sizeof(szDir), szBase, sizeof(szBase), szExt, sizeof(szExt));
		strcpy_s(szSrcDir, sizeof(szSrcDir), szDriver);
		strcat_s(szSrcDir, sizeof(szSrcDir), szDir);

		// delta
		_splitpath_s(szDeltaDir, szDriver, sizeof(szDriver), szDir, sizeof(szDir), szBase, sizeof(szBase), szExt, sizeof(szExt));
		strcpy_s(szDeltaDir, sizeof(szDeltaDir), szDriver);
		strcat_s(szDeltaDir, sizeof(szDeltaDir), szDir);

		// file name
		strcat_s(szBase, sizeof(szBase), szExt);

		return (MergeFileInternal(szSrcDir, szDeltaDir, "\0", szBase, NULL) == TRUE);
	}
	else
	{
		return false;
	}
}

extern "C" __declspec(dllexport) bool MergeDir(LPCWSTR lpszSrcDir, LPCWSTR lpszDeltaDir, LPCWSTR lpszDestDir, PFNMergeDirInfoProc pfn)
{
	if (lpszSrcDir && lpszDeltaDir && pfMD)
	{
		char szSrcDir[1024];
		char szDeltaDir[1024];

		if (!WideCharToMultiByte(CP_ACP,0,lpszSrcDir, -1,szSrcDir,sizeof(szSrcDir),NULL,NULL))
			return 0;
		if (!WideCharToMultiByte(CP_ACP,0,lpszDeltaDir, -1,szDeltaDir,sizeof(szDeltaDir),NULL,NULL))
			return 0;

		char szDeltaDirFull[1024];
		sprintf_s(szDeltaDirFull, sizeof(szDeltaDirFull), "%s\\patch_delta_direct.dat", szDeltaDir);

		if (!pDelFileList)
		{
			pDelFileList = new std::map<std::string, std::string>;
			ParseDirectFileSection(szDeltaDirFull, "DelPathInfo", *pDelFileList);
		}

		if (!pResultMD5List)
		{
			pResultMD5List = new std::map <std::string, std::string>;
			ParseDirectFileSection(szDeltaDirFull, "ResultMD5Info", *pResultMD5List);
		}

		// delete files
		if (pDelFileList)
		{
			PathString strSrcDir(szSrcDir);
			std::map<std::string, std::string>::iterator iter;
			for (iter = pDelFileList->begin(); iter != pDelFileList->end();)
			{
				const PathString& strDeleteFilePath = strSrcDir + PathString("\\") + iter->first;

				if (IsFileExist(strDeleteFilePath))
				{
					bool bSucc = DeleteFile(strDeleteFilePath) ? true : false;
					if (bSucc)
					{
						iter = pDelFileList->erase(iter);
						continue;
					}
				}

				++iter;
			}
		}

		sprintf_s(szDeltaDirFull, sizeof(szDeltaDirFull), "%s\\Pkg", szDeltaDir);
		return (MergeDirInternal(szSrcDir, szDeltaDirFull, "\0", pfn));
	}
	else
	{
		return false;
	}
}

// add by zhangrui
// >>
int GetFileCountInternal(const char* pArchivePath)
{
	int FileCount = 0;

	PathString strArchivePath(pArchivePath);
	HANDLE hArchive = NULL;
	if (SFileOpenArchive(strArchivePath, 0, STREAM_PROVIDER_FLAT, &hArchive))
	{
		check(hArchive);

		SFILE_FIND_DATA fd;
		HANDLE hFind = SFileFindFirstFile(hArchive, "*", &fd, NULL);
		if (hFind)
		{
			do
			{
				++FileCount;
			} while (SFileFindNextFile(hFind, &fd));

			SFileFindClose(hFind);
		}

		SFileCloseArchive(hArchive);
	}

	return FileCount;
}

bool GetFileListInternal(const char* pArchivePath, char pFilePath[][MAX_PATH], unsigned int nMaxCount, unsigned int nCount)
{
	if (!pArchivePath)
	{
		return false;
	}

	if (nCount == 0 || nCount > nMaxCount)
	{
		return false;
	}

	PathString strArchivePath(pArchivePath);
	HANDLE hArchive = NULL;
	if (SFileOpenArchive(strArchivePath, 0, STREAM_PROVIDER_FLAT, &hArchive))
	{
		check(hArchive);

		unsigned int index = 0;
		SFILE_FIND_DATA fd;
		HANDLE hFind = SFileFindFirstFile(hArchive, "*", &fd, NULL);
		if (hFind)
		{
			do
			{
				if (index < nCount)
				{
					size_t nameLength = 0;
					while (nameLength < sizeof(fd.cFileName) && nameLength < MAX_PATH - 1 && fd.cFileName[nameLength])
						++nameLength;
					memcpy(pFilePath[index], fd.cFileName, nameLength);
					pFilePath[index][nameLength] = 0;
					index++;
				}
			} while (SFileFindNextFile(hFind, &fd));

			SFileFindClose(hFind);
		}

		SFileCloseArchive(hArchive);
	}

	return true;
}

DWORD GetFileSizeInternal(const char* pArchivePath, const char* pFilePath)
{
	if (!pArchivePath || !pFilePath)
		return 0;
	HANDLE archive = NULL;
	if (!SFileOpenArchive(pArchivePath, 0, STREAM_PROVIDER_FLAT | STREAM_FLAG_READ_ONLY, &archive))
		return 0;
	DWORD size = 0;
	HANDLE file = NULL;
	if (SFileOpenFileEx(archive, pFilePath, 0, &file))
	{
		DWORD high = 0;
		size = SFileGetFileSize(file, &high);
		if (high != 0 || size == (DWORD)-1)
			size = 0;
		SFileCloseFile(file);
	}
	SFileCloseArchive(archive);
	return size;
}

bool GetFileBufferInternal(const char* pArchivePath, const char* pFilePath,
	unsigned char* pBuffer, DWORD bufferSize, DWORD* pBytesRead)
{
	if (pBytesRead)
		*pBytesRead = 0;
	if (!pArchivePath || !pFilePath || !pBuffer)
		return false;
	HANDLE archive = NULL;
	if (!SFileOpenArchive(pArchivePath, 0, STREAM_PROVIDER_FLAT | STREAM_FLAG_READ_ONLY, &archive))
		return false;
	bool success = false;
	HANDLE file = NULL;
	if (SFileOpenFileEx(archive, pFilePath, 0, &file))
	{
		DWORD high = 0;
		const DWORD size = SFileGetFileSize(file, &high);
		if (high == 0 && size != (DWORD)-1 && size <= bufferSize)
		{
			DWORD read = 0;
			success = SFileReadFile(file, pBuffer, size, &read, NULL) && read == size;
			if (success && pBytesRead)
				*pBytesRead = read;
		}
		SFileCloseFile(file);
	}
	SFileCloseArchive(archive);
	return success;
}

bool DeleteFileInternal(const char* pArchivePath, const char* pFilePath)
{
	bool bSucc = false;
	PathString strArchivePath(pArchivePath);
	HANDLE hArchive = NULL;
	if (SFileOpenArchive(strArchivePath, 0, STREAM_PROVIDER_FLAT, &hArchive))
	{
		check(hArchive);
		bSucc = SFileRemoveFile(hArchive, pFilePath, 0);
		SFileCloseArchive(hArchive);
	}

	return bSucc;
}

bool ExtractFileInternal(const char* pSrcPath, const char* pTempPath, const char* pFilePath, int& iPackIndex)
{
	check(pSrcPath && pTempPath && pFilePath);

	PathString strSrcPath(pSrcPath);
	PathString strFilePath(pFilePath);
	PathString strTempPath(pTempPath);
	ClientPatch::ScopedCurrentDirectory<char> currentDirectory;
	if (!currentDirectory.Change(strTempPath))
		return false;

	std::vector<PathString> vecSrcPacks;
	EnumFilesInDirectory(vecSrcPacks, strSrcPath, PAK_EXT_WITH_POINT);

	return ExtractFromPak2Folder(vecSrcPacks, strFilePath, iPackIndex);
}

bool AddFileInternal(const char* pArchivePath, const char* pFilePath, bool bCreateArchive, DWORD dwNewFileCount)
{
	bool bSucc = false;
	HANDLE hArchive = NULL;
	if (bCreateArchive)
	{
		SFileCreateArchive(pArchivePath, 0, dwNewFileCount, &hArchive);
	}
	else
	{
		SFileOpenArchive(pArchivePath, 0, STREAM_PROVIDER_FLAT, &hArchive);
		if (hArchive)
		{
			if (dwNewFileCount > 0)
			{
				if (!EnsureArchiveCapacity(hArchive, dwNewFileCount))
				{
					SFileCloseArchive(hArchive);
					return false;
				}
			}
		}
	}

	if (hArchive)
	{
		bool bSucc = SFileAddFileEx(hArchive,
			pFilePath,
			pFilePath,
			PAK_CREATE_FLAG, 
			PAK_COMPRESSION, 
			PAK_COMPRESSION_NEXT);
		SFileCloseArchive(hArchive);
		return bSucc;
	}

	return bSucc;
}

extern "C" __declspec(dllexport) int GetPackFileCount(LPCWSTR lpszArchivePath)
{
	if (lpszArchivePath)
	{
		char szArchivePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszArchivePath, -1, szArchivePath, sizeof(szArchivePath), NULL, NULL))
			return 0;
		return GetFileCountInternal(szArchivePath);
	}
	else
	{
		return 0;
	}
}

extern "C" __declspec(dllexport) bool GetPackFileList(LPCWSTR lpszArchivePath, char pFilePath[][MAX_PATH], unsigned int nMaxCount, unsigned int nCount)
{
	if (lpszArchivePath)
	{
		char szArchivePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszArchivePath, -1, szArchivePath, sizeof(szArchivePath), NULL, NULL))
			return 0;
		return GetFileListInternal(szArchivePath, pFilePath, nMaxCount, nCount);
	}
	else
	{
		return false;
	}
}

extern "C" __declspec(dllexport) DWORD GetPackFileSize(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath)
{
	if (lpszArchivePath && lpszFilePath)
	{
		char szArchivePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszArchivePath, -1, szArchivePath, sizeof(szArchivePath), NULL, NULL))
			return 0;

		char szFilePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszFilePath, -1, szFilePath, sizeof(szFilePath), NULL, NULL))
			return 0;

		return GetFileSizeInternal(szArchivePath, szFilePath);
	}
	else
	{
		return 0;
	}
}

extern "C" __declspec(dllexport) bool GetPackFileBuffer(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath, unsigned char* pBuffer)
{
	if (lpszArchivePath && lpszFilePath && pBuffer)
	{
		char szArchivePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszArchivePath, -1, szArchivePath, sizeof(szArchivePath), NULL, NULL))
			return 0;

		char szFilePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszFilePath, -1, szFilePath, sizeof(szFilePath), NULL, NULL))
			return 0;

		// Legacy callers must allocate the full member size. New callers use the sized export.
		return GetFileBufferInternal(szArchivePath, szFilePath, pBuffer, (DWORD)-1, NULL);
	}
	else
	{
		return false;
	}
}

extern "C" __declspec(dllexport) bool GetPackFileBufferSized(LPCWSTR lpszArchivePath,
	LPCWSTR lpszFilePath, unsigned char* pBuffer, DWORD bufferSize)
{
	if (!lpszArchivePath || !lpszFilePath || !pBuffer)
		return false;
	char archivePath[1024], filePath[1024];
	if (!WideCharToMultiByte(CP_ACP, 0, lpszArchivePath, -1, archivePath, sizeof(archivePath), NULL, NULL) ||
		!WideCharToMultiByte(CP_ACP, 0, lpszFilePath, -1, filePath, sizeof(filePath), NULL, NULL))
		return false;
	DWORD read = 0;
	// A changed member must not leave part of the caller's hash input uninitialized.
	return GetFileBufferInternal(archivePath, filePath, pBuffer, bufferSize, &read) && read == bufferSize;
}

extern "C" __declspec(dllexport) bool DeletePackFile(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath)
{
	if (lpszArchivePath && lpszFilePath)
	{
		char szArchivePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszArchivePath, -1, szArchivePath, sizeof(szArchivePath), NULL, NULL))
			return 0;

		char szFilePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszFilePath, -1, szFilePath, sizeof(szFilePath), NULL, NULL))
			return 0;

		return DeleteFileInternal(szArchivePath, szFilePath);
	}
	else
	{
		return false;
	}
}

extern "C" __declspec(dllexport) bool ExtractPackFile(LPCWSTR lpszSrcPath, LPCWSTR lpszTempPath, LPCWSTR lpszFilePath, int& iPackIndex)
{
	if (lpszSrcPath && lpszTempPath && lpszFilePath)
	{
		char szSrcPath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszSrcPath, -1, szSrcPath, sizeof(szSrcPath), NULL, NULL))
			return 0;

		char szTempPath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszTempPath, -1, szTempPath, sizeof(szTempPath), NULL, NULL))
			return 0;

		char szFilePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszFilePath, -1, szFilePath, sizeof(szFilePath), NULL, NULL))
			return 0;

		return ExtractFileInternal(szSrcPath, szTempPath, szFilePath, iPackIndex);
	}
	else
	{
		return false;
	}
}

extern "C" __declspec(dllexport) bool AddPackFile(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath, bool bCreateArchive, DWORD dwNewFileCount)
{
	if (lpszArchivePath && lpszFilePath)
	{
		char szArchivePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszArchivePath, -1, szArchivePath, sizeof(szArchivePath), NULL, NULL))
			return 0;

		char szFilePath[1024];
		if (!WideCharToMultiByte(CP_ACP, 0, lpszFilePath, -1, szFilePath, sizeof(szFilePath), NULL, NULL))
			return 0;

		return AddFileInternal(szArchivePath, szFilePath, bCreateArchive, dwNewFileCount);
	}
	else
	{
		return false;
	}
}

extern "C" __declspec(dllexport) bool MergePackFile(LPCWSTR lpszSrcDir, LPCWSTR lpszDeltaDir, LPCWSTR lpszDestDir)
{
	if (lpszSrcDir && lpszDeltaDir && pfMF)
	{
		return ((*pfMF)(lpszSrcDir, lpszDeltaDir, lpszDestDir));
	}
	else
	{
		return false;
	}
}
// <<

INT MMOMain(const char* pNewVersionPath, const char* pDestDir,
	const char* pRefVersionPath = NULL, const char* pDestPatchDir = NULL)
{
	if (pNewVersionPath == NULL)
	{
		return 0;
	}

	PathString Src(pNewVersionPath);
	PathString Dest(pDestDir != NULL ? pDestDir : "");

	PathString Ref(pRefVersionPath != NULL ? pRefVersionPath : "");
	PathString Patch(pDestPatchDir != NULL ? pDestPatchDir : "");


	OutputInfo(BuildinLog, "/////////////////////////////None-Pak Files/////////////////////////////////\n");

	//////////////////////////////////////////////////////////////////////////
	// none-resource file
	
	std::vector<PathString> VersionFile;

	const UINT nDirCount = NonePakSubDirs.size();

	ClientPatch::ScopedCurrentDirectory<char> currentDirectory;
	if (!currentDirectory.Change(pNewVersionPath))
		return 0;

	for (UINT n = 0; n < nDirCount; ++n)
	{
		EnumFilesInDirectory(VersionFile, NonePakSubDirs[n]);
	}

	for (UINT n = 0; n < VersionFile.size(); ++n)
	{
		const PathString& SubPath = VersionFile[n];

		PathString SrcPath(Src);
		SrcPath += SubPath;

		OutputInfo(BuildinLog, "Processing %s...\n", SrcPath.c_str());

		if (pDestDir != NULL)
		{
			PathString DestPath(Dest);
			DestPath += SubPath;

			CreateDirectoryHierarchy(DestPath);
			BOOL bCopy = CopyFile(SrcPath, DestPath, TRUE);

			GetLastError();
		}
		
		if (pDestPatchDir != NULL && pRefVersionPath != NULL)
		{
			PathString PatchPath(Patch);
			PathString RefPath(Ref);

			PatchPath += SubPath;
			RefPath += SubPath;

			if (CompareFile(SrcPath, RefPath))
			{
				CreateDirectoryHierarchy(PatchPath);
				CopyFile(SrcPath, PatchPath, TRUE);
			}
		}
	}

	currentDirectory.Restore();

	//////////////////////////////////////////////////////////////////////////
	// resource file
	OutputInfo(BuildinLog, "/////////////////////////////Pak Files/////////////////////////////////\n");
	
	for (UINT n = 0; n < PakSubDirs.size(); ++n)
	{
		const PathString& PakDir = PakSubDirs[n];

		PathString S(Src);
		PathString D(Dest);

		PathString R(Ref);
		PathString P(Patch);

		D += PakDir;
		S += PakDir;
		R += PakDir;
		P += PakDir;

		PackFolder(S, (pDestDir ? D.c_str() : NULL), (pRefVersionPath ? R.c_str() : NULL), (pDestPatchDir ? P.c_str() : NULL), NULL, 1073741824, 2147483648, NULL, false, BuildinLog);
	}
	
	OutputInfo(BuildinLog, "/////////////////////////////Finish/////////////////////////////////\n");
		
	return 1;
}

//int _tmain(int argc, _TCHAR* argv[])
//{
//
//}

void InitializePak()
{
	hXDelta3 = ::LoadLibraryEx("XDelta3WrapFactory2.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	if (hXDelta3)
	{
		pfMF = (pfMergeFile)(::GetProcAddress(hXDelta3, "MergeFile"));
		pfMD = (pfMergeDir)(::GetProcAddress(hXDelta3, "MergeDir"));
	}
	
	//__declspec(dllexport) bool MergeFile(LPCWSTR lpszSrcFilePath, LPCWSTR lpszDeltaFilePath, LPCWSTR lpszDestFilePath);

	NonePakSubDirs.push_back("Binaries");
	NonePakSubDirs.push_back("Engine");
	NonePakSubDirs.push_back("history");
	NonePakSubDirs.push_back("SDO");
	NonePakSubDirs.push_back("MMOGame\\Account");
	NonePakSubDirs.push_back("MMOGame\\Content");
	NonePakSubDirs.push_back("MMOGame\\Logs");
	NonePakSubDirs.push_back("MMOGame\\Splash");
	NonePakSubDirs.push_back("MMOGame\\Movies");

	//GetConfigDirs(NonePakSubDirs, "FileConfig.txt");

	PakSubDirs.push_back("MMOGame\\Config");
	PakSubDirs.push_back("MMOGame\\Localization");
	PakSubDirs.push_back("MMOGame\\CookedPC");

	//GetConfigDirs(PakSubDirs, "ResConfig.txt");
}

void FinalizePak()
{
	if (pDelFileList)
	{
		delete pDelFileList;
		pDelFileList = NULL;
	}

	if (pResultMD5List)
	{
		delete pResultMD5List;
		pResultMD5List = NULL;
	}

	FreeLibrary(hXDelta3);
}


INT WINAPI WinMain( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char* pCmdLine, INT nCmdShow )
{
	InitializePak();

	// test case
	/*
	MergeFile(L"D:\\Work\\P4_NEW\\08.Prog_CQ\\Client\\PatchTools\\Bin_Win32\\CQYH\\Binaries\\Ver.ini",
		L"D:\\Work\\P4_NEW\\08.Prog_CQ\\Client\\PatchTools\\Bin_Win32\\CQYH_0.0.1.1-0.0.1.2\\Pkg\\Binaries\\Ver.ini.delta",
		L"D:\\Work\\P4_NEW\\08.Prog_CQ\\Client\\PatchTools\\Bin_Win32\\CQYH\\Binaries\\Ver.ini");
	*/
	
	MergeDir(L"D:\\Work\\P4_NEW\\08.Prog_CQ_S1\\Client\\PatchTools\\Bin_Win32\\1.0.0.6_MPQ\\CQYH",
		L"D:\\Work\\P4_NEW\\08.Prog_CQ_S1\\Client\\PatchTools\\Bin_Win32\\CQYH_1.0.0.6-1.0.0.7",
		L"D:\\Work\\P4_NEW\\08.Prog_CQ_S1\\Client\\PatchTools\\Bin_Win32\\1.0.0.6_MPQ\\CQYH",
		NULL);

	/*
	MergeFileInternal("D:\\Work\\P4_NEW\\08.Prog_CQ_S1\\Client\\PatchTools\\Bin_Win32\\CQYH\\LauncherSkin2.0\\images\\",
		"D:\\Work\\P4_NEW\\08.Prog_CQ_S1\\Client\\PatchTools\\Bin_Win32\\CQYH_1.0.0.2-1.0.0.3\\Pkg\\LauncherSkin2.0\\images\\", 
		"bg_nav.gif.delta",
		NULL);
	*/
	
	/*
	PackFolder("D:\\Work\\P4_NEW\\08.Prog_CQ\\Client\\PatchTools\\Bin_Win32\\CQYH\\07_Quest",
		"D:\\Work\\P4_NEW\\08.Prog_CQ\\Client\\PatchTools\\Bin_Win32\\CQYH", 
		NULL, NULL, NULL, 1024 * 1024);
	*/

	/*
	int count = GetFileCountInternal("D:\\Work\\P4_NEW\\08.Prog_CQ\\Client\\PatchTools\\Bin_Win32\\CQYH\\00000.pak");
	char (*filePath)[MAX_PATH] = new char[count][MAX_PATH];
	GetFileListInternal("D:\\Work\\P4_NEW\\08.Prog_CQ\\Client\\PatchTools\\Bin_Win32\\CQYH\\00000.pak", filePath, count, count);
	delete[]filePath;
	*/

	FinalizePak();

	return 0;

	//pCmdLine = "make D:\\work\\Project\\08.Prog_MPQ2\\Client\\UE3\\MMOGame\\Content\\MMOContent\\08.UI D:\\work\\Project\\08.Prog_MPQ2\\Client\\UE3\\MMOGame\\Content\\MMOContent\\08.UI";
	//pCmdLine = "make D:\\_cook_ok\\MMOGame\\CookedPC\\MMOContent\\08.UI D:\\_cook_ok\\MMOGame\\CookedPC\\MMOContent\\08.UI";
	//pCmdLine = "make D:\\work\\Project\\08.Prog_MPQ2\\Client\\UE3\\MMOGame\\Localization D:\\PATCH";

	std::vector<std::string> vCmdString;
	ParseCmdLine(vCmdString, pCmdLine);

	if (vCmdString.size() > 0)
	{
		//CreateConsole();

		if (vCmdString[0] == "make")
		{
			check(vCmdString.size() > 2);

			PackFolder(vCmdString[1].c_str(), vCmdString[2].c_str(), 
				(vCmdString.size() > 3 ? vCmdString[3].c_str() : NULL), (vCmdString.size() > 4 ? vCmdString[4].c_str() : NULL), 
				(vCmdString.size() > 5 ? vCmdString[5].c_str() : NULL)/*, 100*1024*1024, 200*1024*1024*/);
		}
		else if (vCmdString[0] == "apply")
		{
			ApplyPatch(vCmdString[1].c_str(), vCmdString[2].c_str());
		}
		else if (vCmdString[0] == "compact")
		{
			CompactFolder(vCmdString[1].c_str());

		}
		else if (vCmdString[0] == "ver")
		{
			MMOMain(vCmdString[1].c_str(), vCmdString[2].c_str(), 
				(vCmdString.size() > 3 ? vCmdString[3].c_str() : NULL), (vCmdString.size() > 4 ? vCmdString[4].c_str() : NULL));
		}

		//DestroyConsole();

	}

	return 0;
}


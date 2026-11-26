#ifdef _DEBUG
#define check(express) assert(express)
#else
#define check(express) assert(express)
#endif

#include <set>

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

class PathString : public std::string
{
public:
	PathString()
		: std::string()
	{

	}

	PathString(const char* p)
		: std::string(p)
	{

	}

	PathString(const char* p, UINT len)
		: std::string(p, len)
	{

	}

	PathString(const char* s, const char* e)
		: std::string(s, e)
	{

	}

	PathString(const std::string& rhs)
		: std::string(rhs)
	{

	}

	PathString(const WCHAR* p)
	{
		char szTemp[1024];

		if (!p || !WideCharToMultiByte(CP_ACP, 0, p, -1, szTemp, sizeof(szTemp), NULL, NULL))
			return;

		assign(szTemp);
	}

	PathString(const PathString& rhs)
	{
		assign(rhs);
	}

	operator const char*() const
	{
		return c_str();
	}

	void AppendInt(int n)
	{
		// 强行叠一个数字
		append((const char*)(&n), sizeof(n));
	}

	void AppendStringWithLen(const PathString& s)
	{
		AppendInt(s.size());
		append(s);
	}

	// used for map::find
	//bool operator==(const PakPathString& rhs)
	//{
	//	return true;
	//}

	//bool operator!=(const PakPathString& rhs)
	//{
	//	return true;
	//}

	// >=, >, <=, <

	//const char* c_str() const
	//{
	//	return std::string::c_str();
	//}

	//const WCHAR* C_STR() const
	//{

	//}
};


bool ExtractString(PathString& str, const char*& p, size_t& remaining, size_t len)
{
	if (len > remaining)
		return false;
	str.assign(p, len);
	p += len;
	remaining -= len;
	return true;
}

bool ExtractInt(int& n, const char*& p, size_t& remaining)
{
	if (remaining < sizeof(n))
		return false;
	memcpy(&n, p, sizeof(n));
	p += sizeof(n);
	remaining -= sizeof(n);
	return true;
}

bool ExtractString(PathString& str, const char*& p, size_t& remaining)
{
	int length = 0;
	if (!ExtractInt(length, p, remaining) || length < 0)
		return false;
	return ExtractString(str, p, remaining, (size_t)length);
}

void GetConfigDirs(std::vector<PathString>& VersionSubDirs, const char* pConfigFileName)
{
	FILE* hFile = NULL;

	fopen_s(&hFile, pConfigFileName, "r");

	if (hFile != NULL)
	{
		char szBuff[1024];

		while (fscanf_s(hFile, "%s", szBuff, (unsigned)sizeof(szBuff)) == 1)
		{
			VersionSubDirs.push_back(szBuff);
		}

		fclose(hFile);
	}
}

class DeleteFileMap : public std::map<PathString, std::vector<PathString>>
{
public:
	bool ToString(PathString& data) const
	{
		if (size() > 0)
		{
			data += PATCH_DEL_TABLE_FILE_MAGIC;
			data += PATCH_DEL_TABLE_FILE_VER;

			data.AppendInt(size());

			DeleteFileMap::const_iterator itor;

			for (itor = begin(); itor != end(); ++itor)
			{
				const PathString& Pak = itor->first;
				const std::vector<PathString>& SubPaths = itor->second;

				data.AppendStringWithLen(Pak);

				data.AppendInt(SubPaths.size());
				for (unsigned int p = 0; p < SubPaths.size(); ++p)
				{
					const PathString& DelFileName = SubPaths[p];
					data.AppendStringWithLen(DelFileName);
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	bool FromString(const PathString& data)
	{
		const char* p = data.c_str();
		size_t remaining = data.size();
		PathString temp;
		DeleteFileMap parsed;
		if (!ExtractString(temp, p, remaining, strlen(PATCH_DEL_TABLE_FILE_MAGIC)) ||
			temp != PATCH_DEL_TABLE_FILE_MAGIC ||
			!ExtractString(temp, p, remaining, strlen(PATCH_DEL_TABLE_FILE_VER)) ||
			temp != PATCH_DEL_TABLE_FILE_VER)
			return false;

		int count = 0;
		if (!ExtractInt(count, p, remaining) || count < 0 || (size_t)count > remaining / (2 * sizeof(int)))
			return false;
		for (int i = 0; i < count; ++i)
		{
			int fileCount = 0;
			if (!ExtractString(temp, p, remaining) || !ExtractInt(fileCount, p, remaining) ||
				fileCount < 0 || (size_t)fileCount > remaining / (sizeof(int)))
				return false;
			std::vector<PathString>& files = parsed[temp];
			for (int f = 0; f < fileCount; ++f)
			{
				if (!ExtractString(temp, p, remaining))
					return false;
				files.push_back(temp);
			}
		}
		if (remaining != 0)
			return false;
		swap(parsed);
		return true;
	}
};

class RenameFileMap : public std::map<PathString, std::vector<std::pair<PathString, PathString>>>
{
public:
	bool ToString(PathString& data) const
	{
		if (size() > 0)
		{
			data += PATCH_RENAME_TABLE_FILE_MAGIC;
			data += PATCH_RENAME_TABLE_FILE_VER;

			data.AppendInt(size());

			const_iterator itor;

			for (itor = begin(); itor != end(); ++itor)
			{
				const PathString& Pak = itor->first;
				const std::vector<std::pair<PathString, PathString>>& SubPaths = itor->second;

				data.AppendStringWithLen(Pak);

				data.AppendInt(SubPaths.size());

				for (unsigned int p = 0; p < SubPaths.size(); ++p)
				{
					const std::pair<PathString, PathString>& RenameFile = SubPaths[p];
					data.AppendStringWithLen(RenameFile.first);
					data.AppendStringWithLen(RenameFile.second);
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	bool FromString(const PathString& data)
	{
		const char* p = data.c_str();
		size_t remaining = data.size();
		PathString temp, temp2;
		RenameFileMap parsed;
		if (!ExtractString(temp, p, remaining, strlen(PATCH_RENAME_TABLE_FILE_MAGIC)) ||
			temp != PATCH_RENAME_TABLE_FILE_MAGIC ||
			!ExtractString(temp, p, remaining, strlen(PATCH_RENAME_TABLE_FILE_VER)) ||
			temp != PATCH_RENAME_TABLE_FILE_VER)
			return false;

		int count = 0;
		if (!ExtractInt(count, p, remaining) || count < 0 || (size_t)count > remaining / (2 * sizeof(int)))
			return false;
		for (int i = 0; i < count; ++i)
		{
			int fileCount = 0;
			if (!ExtractString(temp, p, remaining) || !ExtractInt(fileCount, p, remaining) ||
				fileCount < 0 || (size_t)fileCount > remaining / (2 * sizeof(int)))
				return false;
			std::vector<std::pair<PathString, PathString> >& files = parsed[temp];
			for (int f = 0; f < fileCount; ++f)
			{
				if (!ExtractString(temp, p, remaining) || !ExtractString(temp2, p, remaining))
					return false;
				files.push_back(std::make_pair(temp, temp2));
			}
		}
		if (remaining != 0)
			return false;
		swap(parsed);
		return true;
	}
};

#include <atlstr.h>

bool IsDirExist(const LPCSTR dir)
{
	WCHAR szDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, dir, -1, szDir, (int)(sizeof(szDir) / sizeof(szDir[0]))))
		return false;

	DWORD attr;
	attr = GetFileAttributes((LPCWSTR)szDir);

	return ((attr != (DWORD)-1) && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool IsFileExist(const LPCSTR f)
{
	WCHAR szDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, f, -1, szDir, (int)(sizeof(szDir) / sizeof(szDir[0]))))
		return false;

	DWORD attr;
	attr = GetFileAttributes((LPCWSTR)szDir);

	return ((attr != (DWORD)-1) && (!(attr & FILE_ATTRIBUTE_DIRECTORY)));
}

bool mkDir(std::string filePath, bool bRecurse, bool bIncludeLastLayer = false)
{
	//统一路径分隔符
	//filePath.Replace('/', '\\');

	int len = filePath.size();
	if(len < 2)
		return false;

	//去掉最后的斜杠
	while(len != 0 && (filePath[len - 1] == '\\' || filePath[len - 1] == '/'))
	{
		filePath.assign(filePath.c_str(), len - 1);
		len = filePath.size();
	}

	if(len <= 0 )    return false;

	//已经存在直接返回成功
	if(IsDirExist(filePath.c_str()))
	{
		return true;
	}

	//类似G:这样的目录不允许创建
	if(len == 2)    return false;

	//不递归创建，直接返回创建结果
	if(!bRecurse)
	{
		WCHAR szDir[1024];
		if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, filePath.c_str(), -1, szDir, (int)(sizeof(szDir) / sizeof(szDir[0]))))
			return false;

		return CreateDirectory(szDir, NULL) == TRUE;
	}

	//递归创建目录
	std::string topDir ;
	const char* pTail = strchr(filePath.c_str(), '\\');
	if (pTail == NULL)
	{
		pTail = strchr(filePath.c_str(), '/');
	}

	while(NULL != pTail)
	{
		topDir.assign(filePath.c_str(), pTail);

		const char* const pTemp = pTail;
		pTail = strchr(pTail + 1, '\\');
		if (pTail == NULL)
		{
			pTail = strchr(pTemp + 1, '/');
		}

		//存在
		if(IsDirExist(topDir.c_str()))
		{
			continue;
		}

		//不存在
		WCHAR szTopDir[1024];
		if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, topDir.c_str(), -1, szTopDir, (int)(sizeof(szTopDir) / sizeof(szTopDir[0]))))
			return false;
		CreateDirectory(szTopDir, NULL);
	}

	if (bIncludeLastLayer)
	{
		WCHAR szDir[1024];
		if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, filePath.c_str(), -1, szDir, (int)(sizeof(szDir) / sizeof(szDir[0]))))
			return false;
		return CreateDirectory(szDir, NULL) == TRUE;
	}

	return 0;
}

INT CreateDirectoryHierarchy(const char* Path, bool bIncludeLastLayer = false)
{
	if (!Path)
		return 0;
	return (INT)mkDir(Path, true, bIncludeLastLayer);
}

DWORD GetFileSize(const char* FileName)
{
	WCHAR szDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, FileName, -1, szDir, (int)(sizeof(szDir) / sizeof(szDir[0]))))
		return (DWORD)-1;

	HANDLE Handle = CreateFile(szDir, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( Handle==INVALID_HANDLE_VALUE )
	{
		return (DWORD)(-1);
	}

	DWORD Result = GetFileSize( Handle, NULL );
	CloseHandle( Handle );

	return Result;
}

void EnumFilesInDirectory(std::vector<PathString>& files, 
	const char* FolderName, const char* ExtNameWithPoint = NULL, 
	bool bRecursive = true, const std::set<PathString>* ExcludeFiles = NULL)
{
	if (!FolderName || !*FolderName)
		return;
	HANDLE Handle=NULL;

	WIN32_FIND_DATA fd;

	check(strchr(FolderName, '/') == NULL);
	check(*(FolderName + strlen(FolderName) - 1) != '\\');

	PathString strPath(FolderName);

	strPath += "\\*.*";

	WCHAR szPath[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strPath.c_str(), -1, szPath, (int)(sizeof(szPath) / sizeof(szPath[0]))))
		return;
	Handle = FindFirstFile(szPath, &fd);

	if( Handle!=INVALID_HANDLE_VALUE )
	{
		do
		{
			if(fd.cFileName[0] != '.' && (ExcludeFiles == NULL || ExcludeFiles->find(fd.cFileName) == ExcludeFiles->end()))
			{
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bRecursive)
					{
						strPath.assign(FolderName);
						strPath += "\\";
						strPath += PathString(fd.cFileName);

						EnumFilesInDirectory(files, strPath.c_str(), ExtNameWithPoint, bRecursive);
					}
				}
				else if (ExtNameWithPoint == NULL || strstr(PathString(fd.cFileName).c_str(), ExtNameWithPoint) != NULL)
				{
					strPath = FolderName;
					strPath += "\\";
					strPath += PathString(fd.cFileName);

					files.push_back(strPath);
				}
			}

		}
		while( FindNextFile(Handle,&fd) );

		FindClose( Handle );
	}
}

void DeleteFilesInDirectory(const char* FolderName, bool bRecursive)
{
	if (!FolderName || !*FolderName)
		return;
	/*
	char szCmd[1024];
	sprintf_s(szCmd, sizeof(szCmd), "del /s/q %s", FolderName);
	system(szCmd);

	sprintf_s(szCmd, sizeof(szCmd), "rd /s/q %s", FolderName);
	system(szCmd);
	*/

	HANDLE Handle = NULL;
	WIN32_FIND_DATA fd;

	check(strchr(FolderName, '/') == NULL);
	check(*(FolderName + strlen(FolderName) - 1) != '\\');

	PathString strPath(FolderName);
	strPath += "\\*.*";

	WCHAR szPath[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strPath.c_str(), -1, szPath, (int)(sizeof(szPath) / sizeof(szPath[0]))))
		return;

	Handle = FindFirstFile(szPath, &fd);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.cFileName[0] != '.')
			{
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bRecursive)
					{
						strPath.assign(FolderName);
						strPath += PathString("\\");
						strPath += PathString(fd.cFileName);

						DeleteFilesInDirectory(strPath.c_str(), bRecursive);

						if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strPath.c_str(), -1, szPath, (int)(sizeof(szPath) / sizeof(szPath[0]))))
							continue;
						::RemoveDirectory(szPath);
					}
				}
				else
				{
					strPath = FolderName;
					strPath += PathString("\\");
					strPath += PathString(fd.cFileName);

					if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strPath.c_str(), -1, szPath, (int)(sizeof(szPath) / sizeof(szPath[0]))))
						continue;

					if (IsFileExist(strPath))
					{// 去除文件的只读属性， 否则会删除失败
						DWORD attr = ::GetFileAttributes((LPCTSTR)szPath);
						attr &= ~FILE_ATTRIBUTE_READONLY;
						::SetFileAttributes((LPCTSTR)szPath, attr);
					}

					::DeleteFile(szPath);
				}
			}

		} while (FindNextFile(Handle, &fd));

		FindClose(Handle);
	}
}

class MPQPath
{

public:
	PathString MPQFilePath;		// where is the mpq locate in the os
	PathString InMPQPath;		// where is the file locate in the mpq, the name in MPQ

	MPQPath()
	{

	}

	MPQPath(const char* szMPQFilePath, const char* szInMPQPath)
		: MPQFilePath(szMPQFilePath)
		, InMPQPath(szInMPQPath)
	{

	}

};

class PakFileInfo
{
public:
	PathString SrcFileName;

	PathString RefMPQFilePath;
	PathString RefInMPQPath;

public:
	PakFileInfo()
	{

	}

	PakFileInfo(const PathString& SrcFile)
		: SrcFileName(SrcFile)
	{

	}

	PakFileInfo(const PathString& SrcFile, const PathString& MPQ, const PathString& InMPQ)
		: SrcFileName(SrcFile)
		, RefMPQFilePath(MPQ)
		, RefInMPQPath(InMPQ)
	{

	}
};

class ArchiveInfo
{
public:

	UINT ArchiveRawSize;
	// patch size must be smaller or equal than size

	union
	{
		struct  
		{
			//HANDLE hArchive;
			//HANDLE hPatchArchive;

			std::vector<PathString> EntireFiles;
			std::vector<PathString> PatchFiles;

			std::vector<PathString> EntirePathsInMPQ;
			std::vector<PathString> PatchPathsInMPQ;
		};

		struct
		{
			//HANDLE hArchives[2];
			std::vector<PathString> Files[2];
			std::vector<PathString> PathsInMPQ[2];
		};
	};

	ArchiveInfo()
	{
		//hArchive = NULL;
		ArchiveRawSize = 0;


		//hPatchArchive = NULL;
	}

	void AddFile(const char* pFile, const char* pRelativePathInMPQ, UINT nFileSize = 0)
	{
		check(pFile && pRelativePathInMPQ);

		EntireFiles.push_back(pFile);
		EntirePathsInMPQ.push_back(pRelativePathInMPQ);

		check(0 == nFileSize || nFileSize == GetFileSize(pFile));

		ArchiveRawSize += (nFileSize == 0 ? GetFileSize(pFile) : nFileSize);
	}

	void AddPatchFile(const char* pFile, const char* pRelativePathInMPQ)
	{
		check(pFile && pRelativePathInMPQ);

		PatchFiles.push_back(pFile);
		PatchPathsInMPQ.push_back(pRelativePathInMPQ);
	}
};

void PushDeleteFile(DeleteFileMap& DeleteFiles, const MPQPath& rPath)
{
	PathString key(rPath.MPQFilePath.c_str() + rPath.MPQFilePath.size() - PAK_NAME_LEN);
	DeleteFiles[key].push_back(rPath.InMPQPath);
}

void PushRenameFile(RenameFileMap& RenameFiles, 
	const MPQPath& rPath, const PathString& NewInMPQPath)
{
	PathString key(rPath.MPQFilePath.c_str() + rPath.MPQFilePath.size() - PAK_NAME_LEN);
	RenameFiles[key].push_back(std::pair<PathString, PathString>(rPath.InMPQPath, NewInMPQPath));
}

#include <xiosbase>
#include <fstream>

#include "Markup.h"

bool ParseDirectFileSection(LPCTSTR lpszDirectFile, LPCTSTR lpszSectionName, std::map<std::string, std::string> &mapKeyValue)
{
	CMarkup markup;
	bool bSucceed = markup.Load(lpszDirectFile);
	if (bSucceed)
	{
		bSucceed = markup.FindElem();
		if (bSucceed)
		{
			bSucceed = markup.IntoElem();
			if (bSucceed)
			{
				bSucceed = markup.FindElem(lpszSectionName);
				if (bSucceed)
				{
					markup.IntoElem();
					while(markup.FindElem())
					{
						WCHAR szKey[MAX_PATH];
						if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)STR_DIRECT_FILE_SUB_ITEM_KEY_ATTRIB, -1, szKey, MAX_PATH))
							return false;

						WCHAR szValue[MAX_PATH];
						if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)STR_DIRECT_FILE_SUB_ITEM_VALUE_ATTRIB, -1, szValue, MAX_PATH))
							return false;

						char szKeyAttr[MAX_PATH];
						if (!WideCharToMultiByte(CP_ACP, 0, markup.GetAttrib(szKey), -1, szKeyAttr, sizeof(szKeyAttr), NULL, NULL))
							return false;

						char szValueAttr[MAX_PATH];
						if (!WideCharToMultiByte(CP_ACP, 0, markup.GetAttrib(szValue), -1, szValueAttr, sizeof(szValueAttr), NULL, NULL))
							return false;

						mapKeyValue[std::string(szKeyAttr)] = std::string(szValueAttr);
					}
					markup.OutOfElem();
				}
			}
			markup.OutOfElem();
		}
	}
	return bSucceed;
}
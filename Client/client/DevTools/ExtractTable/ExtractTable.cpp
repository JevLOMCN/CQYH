// ExtractTable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <map>
#include <conio.h>
#include "tabfile.h"
#include "utf8.h"
#include <windows.h>
#include <limits.h>

std::wstring TrimQuotes(const std::wstring& str, bool* bQuotesRemoved=NULL )
{
	bool bQuotesWereRemoved=false;
	size_t Start = 0, Count = str.length();
	if ( Count > 0 )
	{
		if ( str[0] == wchar_t('"') )
		{
			Start++;
			Count--;
			bQuotesWereRemoved=true;
		}

		if ( str.length() > 1 && str[str.length() - 1] == wchar_t('"') )
		{
			Count--;
			bQuotesWereRemoved=true;
		}
	}

	if ( bQuotesRemoved != NULL )
	{
		*bQuotesRemoved = bQuotesWereRemoved;
	}
	return str.substr(Start, Count);
}

typedef std::map<std::wstring, int> HeaderIndexMap;
HeaderIndexMap header;

int GetHdeaderIndex(const std::wstring& headerName)
{
	HeaderIndexMap::iterator i = header.find(headerName);
	if(i != header.end())
	{
		return i->second;
	}

	std::wcout << L"没找到这个表头的索引:" << headerName << std::endl;
	return 0;
}


static bool AppendUtf8TableCell(std::wstring& output, const char* text)
{
    if (!text)
        return false;
    const size_t bytes = strlen(text);
    if (bytes == 0)
        return true;
    if (bytes > INT_MAX)
        return false;
    const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, (int)bytes, NULL, 0);
    if (length <= 0)
        return false;
    std::wstring converted(length, L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, (int)bytes, &converted[0], length) != length)
        return false;
    output.append(converted);
    return true;
}

int _tmain(int argc, wchar_t* argv[])
{
	setlocale(0, "CHS");

	if(argc < 3)
	{
		std::wcout << L"使用方法：ExtractTable srcFile destFile header1 header2 header3 header4..." << std::endl;
		std::wcout << L"Press any key to quit...";
		while(!_kbhit())
		{
			::_sleep(10);
		}
		return -1;
	}

	typedef std::vector<std::wstring> StringVector;
	typedef std::vector<StringVector> StringArray2D;

	StringVector headerList;
	StringArray2D strArr2D;

	std::wstring srcFile, destFile;
	srcFile = argv[1];
	destFile = argv[2];

	for (int i = 3; i < argc; ++i)
	{
		headerList.push_back(argv[i]);
	}

	std::wcout << L"正在读取：" << srcFile << std::endl;

	CTabFile tabFile;
	if(!tabFile.Init(srcFile.c_str()))
	{
		return -1;
	}

	if(tabFile.GetRowCount() <= 2)
	{
		std::wcout << L"只读取了不超过2行数据，说明只有表头和默认值行，源文件可能有问题:" << srcFile << std::endl;
		return -3;
	}

	//std::wcout << L"正在保存：" << destFile << std::endl;
	std::wcout << L"正在保存：" << destFile << L" 共 " << tabFile.GetRowCount() << L" 行" << std::endl;

	typedef std::vector<int> IntVector;
	IntVector requiredCols;
	for(size_t k = 0; k < tabFile.GetColCount(); ++k)
	{
		const char* buf = tabFile.GetData(k, 1);
		//std::wcout << buf << L" " << tabFile.GetData(k, 0);
		if((stricmp(buf, "Client") == 0) || (stricmp(buf, "Both") == 0))
		{
			//std::wcout << "+";
			requiredCols.push_back(k);
		}
		//std::wcout << std::endl;
	}

	std::wcout << L"提取了：" << requiredCols.size() << L"列， 共:" << tabFile.GetColCount() << L" 列" << std::endl;

	std::wstring writeBuf;
	for (size_t j = 0; j < tabFile.GetRowCount(); ++j)
	{		
		for (size_t i = 0; i < requiredCols.size(); ++i)
		{
			const char* buf = tabFile.GetData(requiredCols[i], j);
			
			if(buf)
			{
				if (!AppendUtf8TableCell(writeBuf, buf))
					return -4;
			}
			else
			{
				writeBuf += L" ";
			}

			if(i < requiredCols.size() - 1)
			{
				writeBuf += std::wstring(L"\t");
			}
		}

		writeBuf += std::wstring(L"\r\n");
	}

#if 0
	std::ofstream ofs(destFile, std::ios::out|std::ios::bin);
	char t = 0xff;
	ofs.write(&t, sizeof(char));

	t = 0xfe;
	ofs.write(&t, sizeof(char));

	ofs.write((char*)&writeBuf[0], writeBuf.size() * 2);
	ofs.close();
#else
	FILE* fp = _wfopen(destFile.c_str(), L"wb+");
	if(fp)
	{
		wchar_t t(0xFEFF);
		wchar_t sep(L'\t');
		std::wstring rt(L"\r\n");

		fwrite(&t, sizeof(wchar_t), 1, fp);

		fwrite((const void*)&writeBuf[0], writeBuf.size() * sizeof(wchar_t), 1, fp);
		fclose(fp);
	}
#endif

	std::wcout << L"保存完成：" << destFile << std::endl;

	return 0;
}


//// Bruce@snda 
// encry.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <direct.h>
#include <errno.h>

typedef unsigned char       BYTE;

class FArchiveEncodeDncode
{
public:
	BYTE  offsetEnCode[256];
	BYTE  offsetDeCode[256]; 

	FArchiveEncodeDncode()
	{
		int	 seedEncode = 897502;
		for ( int i=0; i<256; i++ )
		{
			offsetEnCode[i] = i;
		}

		srand( seedEncode );
		BYTE  buf;
		int  iBuf;
		for(int i=0;i<256;i++)
		{
			iBuf = rand()%256; 
			buf  = offsetEnCode[i];
			offsetEnCode[i]		= offsetEnCode[iBuf];
			offsetEnCode[iBuf]	= buf;
		}

		for(int i=0;i<256;i++)
		{
			offsetDeCode[offsetEnCode[i]] = i;
		}
	};

	~FArchiveEncodeDncode()
	{
	};

	static FArchiveEncodeDncode* GetInstance()
	{
		static FArchiveEncodeDncode ins;
		return &ins;
	};

	void DoEncode(void* V, int Length)
	{
		BYTE* pData = (BYTE*)V;

		for ( int i = 0; i < Length; i++ )
		{
			*(pData+i) = offsetEnCode[*(pData+i)];
		}
	}

	void DoDecode(void* V, int Length)
	{
		BYTE* pData = (BYTE*)V;

		for ( int i=0; i<Length; i++ )
		{
			*(pData+i) = offsetDeCode[*(pData+i)];
		}
	}
};

#define PATH_SEPARATOR _T("\\")
#define appIsPathSeparator( Ch )	((Ch) == PATH_SEPARATOR[0])


bool _MakeDirectory( const TCHAR* Path, bool Tree = true)
{
	if (!Path || !*Path)
		return false;
	if (!Tree)
		return _tmkdir(Path) == 0 || errno == EEXIST;
	if (_tcslen(Path) >= 256)
		return false;

	// Support code for making a directory tree.
	//check(Tree);
	int CreateCount=0;
	for( TCHAR Full[256]=_T(""), *Ptr=Full; ; *Ptr++=*Path++ )
	{
		if( appIsPathSeparator(*Path) || *Path==0 )
		{
			*Ptr = 0;
			if( Ptr != Full )// && !IsDrive(Full) )
			{
				if( !_MakeDirectory( Full, 0 ) )
				{
					return 0;
				}
				CreateCount++;
			}
		}
		if( *Path==0 )
		{
			break;
		}
	}
	return CreateCount!=0;
}

int _tmain(int argc, TCHAR* argv[])
{
	if (argc < 2 || !argv || !argv[1] || (argc > 2 && !argv[2]))
	{
		_tprintf(L"Usage: encry <input> [output] [decode]\n");
		return -1;
	}

	std::wstring outputName = argv[1];
	if (argc == 2)
		outputName += L".txt";
	else
		outputName = argv[2];

	FILE* OriginFile = NULL;
	FILE* DestiFile = NULL;
	char* buffer = NULL;
	int result = -1;
	_tfopen_s(&OriginFile, argv[1], L"rb");
	if (!OriginFile)
	{
		_tprintf(L"[Error] Original File: Open file %ls Error!\n", argv[1]);
		return -1;
	}

	do
	{
		if (fseek(OriginFile, 0, SEEK_END) != 0)
			break;
		const long size = ftell(OriginFile);
		if (size < 0 || fseek(OriginFile, 0, SEEK_SET) != 0)
			break;

		buffer = (char*)malloc(size > 0 ? (size_t)size : 1);
		if (!buffer || fread(buffer, 1, (size_t)size, OriginFile) != (size_t)size)
			break;
		const int closeResult = fclose(OriginFile);
		OriginFile = NULL;
		if (closeResult != 0)
			break;

		FArchiveEncodeDncode* encode = FArchiveEncodeDncode::GetInstance();
		if (argc == 2 || argc > 3)
			encode->DoDecode(buffer, size);
		else
			encode->DoEncode(buffer, size);

		_tfopen_s(&DestiFile, outputName.c_str(), L"wb+");
		if (!DestiFile)
			break;
		if (fwrite(buffer, 1, (size_t)size, DestiFile) != (size_t)size)
			break;
		const int outputCloseResult = fclose(DestiFile);
		DestiFile = NULL;
		if (outputCloseResult != 0)
			break;
		result = 0;
	} while (false);

	if (OriginFile)
		fclose(OriginFile);
	if (DestiFile)
		fclose(DestiFile);
	free(buffer);

	if (result == 0)
		_tprintf(L"[OK] %ls\r\n", outputName.c_str());
	else
		_tprintf(L"[Error] Could not process %ls\n", argv[1]);
	return result;
}



//int  _tmain(int argc, _TCHAR* argv[])
//{
//	FILE* OriginFile;
//	FILE* DestiFile;
//
//	int FileLength = 0;
//	if ( ( OriginFile = fopen( argv[1],"rb+" ) ) == NULL )
//	{
//		printf("Original File: Open file %s Error! \n", argv[1]);
//		getch();
//		return -1;
//	}
//	while ( !feof( OriginFile ) )
//	{
//		unsigned char ch = fgetc( OriginFile );
//		FileLength++;		
//	}
//	fclose( OriginFile );
//
//	if ( ( OriginFile = fopen( argv[1],"rb+" ) ) == NULL )
//	{
//		printf("Original File: Open file %s Error! \n", argv[1]);
//		getch();
//		return -1;
//	}
//
//	if ( ( DestiFile = fopen( argv[2],"wb+" ) ) == NULL )
//	{
//		printf("Temporary File: Create file %s Error !\n", argv[2]);
//		getch();
//		return -1;
//	}
//
//	FArchiveEncodeDncode* pEncode = FArchiveEncodeDncode::GetInstance();
//	int EncodeLen = 0;
//	while ( !feof( OriginFile ) )
//	{
//		unsigned char ch = fgetc( OriginFile );
//		EncodeLen++;
//
//		if ( FileLength == EncodeLen )
//			break;
//
//		ch = pEncode->offsetEnCode[(int)ch];		
//		//ch = pEncode->offsetDeCode[(int)ch];	
//
//		fputc( ch, DestiFile );
//
//	}
//
//	fclose( OriginFile );
//	fclose( DestiFile );
//
//	getch();
//	return 0;
//}

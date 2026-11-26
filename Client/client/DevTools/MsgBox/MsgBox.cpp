// MsgBox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
		return 1;
	::MessageBox(NULL, argv[1], argv[2], MB_OK);
	return 0;
}


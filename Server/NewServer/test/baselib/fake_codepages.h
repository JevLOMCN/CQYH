#pragma once
#include <Windows.h>

int WINAPI TestMultiByteToWideChar(UINT, DWORD, LPCCH, int, LPWSTR, int);
int WINAPI TestWideCharToMultiByte(UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL);

#define MultiByteToWideChar TestMultiByteToWideChar
#define WideCharToMultiByte TestWideCharToMultiByte

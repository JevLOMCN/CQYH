## {{{ http://code.activestate.com/recipes/305279/ (r4)
"""
Enumerates active processes as seen under windows Task Manager on Win NT/2k/XP using PSAPI.dll
(new api for processes) and using ctypes.Use it as you please.

Based on information from http://support.microsoft.com/default.aspx?scid=KB;EN-US;Q175030&ID=KB;EN-US;Q175030

By Eric Koome
email ekoome@yahoo.com
license GPL
"""
from ctypes import *
from ctypes import wintypes

# ctypes defaults to int without signatures, truncating 64-bit handles.
psapi = windll.psapi
kernel = windll.kernel32
psapi.EnumProcesses.argtypes = [POINTER(wintypes.DWORD), wintypes.DWORD, POINTER(wintypes.DWORD)]
psapi.EnumProcesses.restype = wintypes.BOOL
psapi.EnumProcessModules.argtypes = [wintypes.HANDLE, POINTER(wintypes.HMODULE), wintypes.DWORD, POINTER(wintypes.DWORD)]
psapi.EnumProcessModules.restype = wintypes.BOOL
psapi.GetModuleBaseNameA.argtypes = [wintypes.HANDLE, wintypes.HMODULE, c_void_p, wintypes.DWORD]
psapi.GetModuleBaseNameA.restype = wintypes.DWORD
kernel.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
kernel.OpenProcess.restype = wintypes.HANDLE
kernel.CloseHandle.argtypes = [wintypes.HANDLE]
kernel.CloseHandle.restype = wintypes.BOOL


def EnumProcesses():
    pp = {}
    capacity = 256
    while True:
        process_ids = (wintypes.DWORD * capacity)()
        needed = wintypes.DWORD()
        if not psapi.EnumProcesses(process_ids, sizeof(process_ids), byref(needed)):
            return pp
        if needed.value < sizeof(process_ids):
            break
        capacity *= 2

    for pid in process_ids[:needed.value // sizeof(wintypes.DWORD)]:
        handle = kernel.OpenProcess(0x0400 | 0x0010, False, pid)
        if not handle:
            continue
        try:
            module = wintypes.HMODULE()
            module_bytes = wintypes.DWORD()
            if not psapi.EnumProcessModules(handle, byref(module), sizeof(module), byref(module_bytes)):
                continue
            if module_bytes.value < sizeof(module):
                continue
            name = create_string_buffer(32768)
            length = psapi.GetModuleBaseNameA(handle, module, name, sizeof(name))
            if not length or length >= sizeof(name):
                continue
            pp[name.raw[:length]] = pid
        finally:
            kernel.CloseHandle(handle)
    return pp


if __name__ == '__main__':
    print(EnumProcesses())

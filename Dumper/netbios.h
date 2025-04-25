#include <Windows.h>

FARPROC Real_Netbios = NULL;

HMODULE Real_Module = NULL;

FARPROC WINAPI GetAddress(LPCSTR lpProcName)
{
    FARPROC pAddress = GetProcAddress(Real_Module, lpProcName);
    if (pAddress == NULL)
    {
        MessageBoxW(NULL, L"Get address failed", L"netbios", MB_OK);
        ExitProcess(1);
    }
    return pAddress;
}

VOID WINAPI FreeHijack()
{{
    if (Real_Module != NULL)
        FreeLibrary(Real_Module);
    return;
}}

VOID WINAPI InitHijack()
{
    WCHAR real_dll_path[MAX_PATH];
    GetSystemDirectoryW(real_dll_path, MAX_PATH);
    lstrcatW(real_dll_path, L"\\netbios.dll");

    Real_Module = LoadLibraryW(real_dll_path);
    if (Real_Module == NULL)
    {
        MessageBoxW(NULL, L"Load original dll failed", L"netbios", MB_OK);
        ExitProcess(1);
    }

    Real_Netbios = GetAddress("Netbios");
}

void Redirect_Netbios() { Real_Netbios(); }

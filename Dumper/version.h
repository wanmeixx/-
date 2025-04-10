#include <Windows.h>

FARPROC Real_GetFileVersionInfoA = NULL;
FARPROC Real_GetFileVersionInfoByHandle = NULL;
FARPROC Real_GetFileVersionInfoExA = NULL;
FARPROC Real_GetFileVersionInfoExW = NULL;
FARPROC Real_GetFileVersionInfoSizeA = NULL;
FARPROC Real_GetFileVersionInfoSizeExA = NULL;
FARPROC Real_GetFileVersionInfoSizeExW = NULL;
FARPROC Real_GetFileVersionInfoSizeW = NULL;
FARPROC Real_GetFileVersionInfoW = NULL;
FARPROC Real_VerFindFileA = NULL;
FARPROC Real_VerFindFileW = NULL;
FARPROC Real_VerInstallFileA = NULL;
FARPROC Real_VerInstallFileW = NULL;
FARPROC Real_VerLanguageNameA = NULL;
FARPROC Real_VerLanguageNameW = NULL;
FARPROC Real_VerQueryValueA = NULL;
FARPROC Real_VerQueryValueW = NULL;

HMODULE Real_Module = NULL;

FARPROC WINAPI GetAddress(LPCSTR lpProcName)
{
    FARPROC pAddress = GetProcAddress(Real_Module, lpProcName);
    if (pAddress == NULL)
    {
        MessageBoxW(NULL, L"Get address failed", L"version", MB_OK);
        ExitProcess(1);
    }
    return pAddress;
}

VOID WINAPI FreeHijack()
{
    if (Real_Module != NULL)
        FreeLibrary(Real_Module);
    return;
}

VOID WINAPI InitHijack()
{
    WCHAR real_dll_path[MAX_PATH];
    GetSystemDirectoryW(real_dll_path, MAX_PATH);
    lstrcatW(real_dll_path, L"\\version.dll");

    Real_Module = LoadLibraryW(real_dll_path);
    if (Real_Module == NULL)
    {
        MessageBoxW(NULL, L"Load original dll failed", L"version", MB_OK);
        ExitProcess(1);
    }

    Real_GetFileVersionInfoA = GetAddress("GetFileVersionInfoA");
    Real_GetFileVersionInfoByHandle = GetAddress("GetFileVersionInfoByHandle");
    Real_GetFileVersionInfoExA = GetAddress("GetFileVersionInfoExA");
    Real_GetFileVersionInfoExW = GetAddress("GetFileVersionInfoExW");
    Real_GetFileVersionInfoSizeA = GetAddress("GetFileVersionInfoSizeA");
    Real_GetFileVersionInfoSizeExA = GetAddress("GetFileVersionInfoSizeExA");
    Real_GetFileVersionInfoSizeExW = GetAddress("GetFileVersionInfoSizeExW");
    Real_GetFileVersionInfoSizeW = GetAddress("GetFileVersionInfoSizeW");
    Real_GetFileVersionInfoW = GetAddress("GetFileVersionInfoW");
    Real_VerFindFileA = GetAddress("VerFindFileA");
    Real_VerFindFileW = GetAddress("VerFindFileW");
    Real_VerInstallFileA = GetAddress("VerInstallFileA");
    Real_VerInstallFileW = GetAddress("VerInstallFileW");
    Real_VerLanguageNameA = GetAddress("VerLanguageNameA");
    Real_VerLanguageNameW = GetAddress("VerLanguageNameW");
    Real_VerQueryValueA = GetAddress("VerQueryValueA");
    Real_VerQueryValueW = GetAddress("VerQueryValueW");
}

void Redirect_GetFileVersionInfoA() { Real_GetFileVersionInfoA(); }
void Redirect_GetFileVersionInfoByHandle() { Real_GetFileVersionInfoByHandle(); }
void Redirect_GetFileVersionInfoExA() { Real_GetFileVersionInfoExA(); }
void Redirect_GetFileVersionInfoExW() { Real_GetFileVersionInfoExW(); }
void Redirect_GetFileVersionInfoSizeA() { Real_GetFileVersionInfoSizeA(); }
void Redirect_GetFileVersionInfoSizeExA() { Real_GetFileVersionInfoSizeExA(); }
void Redirect_GetFileVersionInfoSizeExW() { Real_GetFileVersionInfoSizeExW(); }
void Redirect_GetFileVersionInfoSizeW() { Real_GetFileVersionInfoSizeW(); }
void Redirect_GetFileVersionInfoW() { Real_GetFileVersionInfoW(); }
void Redirect_VerFindFileA() { Real_VerFindFileA(); }
void Redirect_VerFindFileW() { Real_VerFindFileW(); }
void Redirect_VerInstallFileA() { Real_VerInstallFileA(); }
void Redirect_VerInstallFileW() { Real_VerInstallFileW(); }
void Redirect_VerLanguageNameA() { Real_VerLanguageNameA(); }
void Redirect_VerLanguageNameW() { Real_VerLanguageNameW(); }
void Redirect_VerQueryValueA() { Real_VerQueryValueA(); }
void Redirect_VerQueryValueW() { Real_VerQueryValueW(); }

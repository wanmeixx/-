#include <Windows.h>

FARPROC Real_CreateDXGIFactory = NULL;
FARPROC Real_CreateDXGIFactory1 = NULL;
FARPROC Real_CreateDXGIFactory2 = NULL;
FARPROC Real_DXGID3D10CreateDevice = NULL;
FARPROC Real_DXGID3D10CreateLayeredDevice = NULL;
FARPROC Real_DXGID3D10ETWRdtscFrequency = NULL;
FARPROC Real_DXGID3D10GetLayeredDeviceSize = NULL;
FARPROC Real_DXGID3D10RegisterLayers = NULL;
FARPROC Real_DXGIDeclareAdapterRemovalSupport = NULL;
FARPROC Real_DXGIDumpJournal = NULL;
FARPROC Real_DXGIGetDebugInterface1 = NULL;
FARPROC Real_DXGIReportAdapterConfiguration = NULL;

HMODULE Real_Module = NULL;

FARPROC WINAPI GetAddress(LPCSTR lpProcName)
{
    // Return NULL for exports the real dxgi.dll does not provide instead of
    // aborting the host process. Any export the host actually imports will be
    // present in the real dll; optional ones just stay NULL and are never called.
    return GetProcAddress(Real_Module, lpProcName);
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
    lstrcatW(real_dll_path, L"\\dxgi.dll");

    Real_Module = LoadLibraryW(real_dll_path);
    if (Real_Module == NULL)
    {
        MessageBoxW(NULL, L"Load original dll failed", L"dxgi", MB_OK);
        ExitProcess(1);
    }

    Real_CreateDXGIFactory = GetAddress("CreateDXGIFactory");
    Real_CreateDXGIFactory1 = GetAddress("CreateDXGIFactory1");
    Real_CreateDXGIFactory2 = GetAddress("CreateDXGIFactory2");
    Real_DXGID3D10CreateDevice = GetAddress("DXGID3D10CreateDevice");
    Real_DXGID3D10CreateLayeredDevice = GetAddress("DXGID3D10CreateLayeredDevice");
    Real_DXGID3D10ETWRdtscFrequency = GetAddress("DXGID3D10ETWRdtscFrequency");
    Real_DXGID3D10GetLayeredDeviceSize = GetAddress("DXGID3D10GetLayeredDeviceSize");
    Real_DXGID3D10RegisterLayers = GetAddress("DXGID3D10RegisterLayers");
    Real_DXGIDeclareAdapterRemovalSupport = GetAddress("DXGIDeclareAdapterRemovalSupport");
    Real_DXGIDumpJournal = GetAddress("DXGIDumpJournal");
    Real_DXGIGetDebugInterface1 = GetAddress("DXGIGetDebugInterface1");
    Real_DXGIReportAdapterConfiguration = GetAddress("DXGIReportAdapterConfiguration");
}

void Redirect_CreateDXGIFactory() { Real_CreateDXGIFactory(); }
void Redirect_CreateDXGIFactory1() { Real_CreateDXGIFactory1(); }
void Redirect_CreateDXGIFactory2() { Real_CreateDXGIFactory2(); }
void Redirect_DXGID3D10CreateDevice() { Real_DXGID3D10CreateDevice(); }
void Redirect_DXGID3D10CreateLayeredDevice() { Real_DXGID3D10CreateLayeredDevice(); }
void Redirect_DXGID3D10ETWRdtscFrequency() { Real_DXGID3D10ETWRdtscFrequency(); }
void Redirect_DXGID3D10GetLayeredDeviceSize() { Real_DXGID3D10GetLayeredDeviceSize(); }
void Redirect_DXGID3D10RegisterLayers() { Real_DXGID3D10RegisterLayers(); }
void Redirect_DXGIDeclareAdapterRemovalSupport() { Real_DXGIDeclareAdapterRemovalSupport(); }
void Redirect_DXGIDumpJournal() { Real_DXGIDumpJournal(); }
void Redirect_DXGIGetDebugInterface1() { Real_DXGIGetDebugInterface1(); }
void Redirect_DXGIReportAdapterConfiguration() { Real_DXGIReportAdapterConfiguration(); }

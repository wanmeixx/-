#include <windows.h>
#include <Shlwapi.h>
#include <iostream>
#include <chrono>
#include <fstream>

#include "CppGenerator.h"
#include "MappingGenerator.h"
#include "IDAMappingGenerator.h"
#include "DumpspaceGenerator.h"

#include "StructManager.h"
#include "EnumManager.h"

#include "Generator.h"


#include "UnicodeNames.h"

enum class EFortToastType : uint8
{
    Default = 0,
    Subdued = 1,
    Impactful = 2,
    EFortToastType_MAX = 3,
};

#pragma comment(lib, "Shlwapi.lib")
//#pragma comment(linker, "/EXPORT:Netbios=AheadLibEx_Netbios,@1")

#pragma comment(linker, "/EXPORT:KsCreateAllocator=AheadLibEx_KsCreateAllocator,@1")
#pragma comment(linker, "/EXPORT:KsCreateAllocator2=AheadLibEx_KsCreateAllocator2,@2")
#pragma comment(linker, "/EXPORT:KsCreateClock=AheadLibEx_KsCreateClock,@3")
#pragma comment(linker, "/EXPORT:KsCreateClock2=AheadLibEx_KsCreateClock2,@4")
#pragma comment(linker, "/EXPORT:KsCreatePin=AheadLibEx_KsCreatePin,@5")
#pragma comment(linker, "/EXPORT:KsCreatePin2=AheadLibEx_KsCreatePin2,@6")
#pragma comment(linker, "/EXPORT:KsCreateTopologyNode=AheadLibEx_KsCreateTopologyNode,@7")
#pragma comment(linker, "/EXPORT:KsCreateTopologyNode2=AheadLibEx_KsCreateTopologyNode2,@8")

extern "C" {
    //PVOID pfnAheadLibEx_Netbios;

    PVOID pfnAheadLibEx_KsCreateAllocator;
    PVOID pfnAheadLibEx_KsCreateAllocator2;
    PVOID pfnAheadLibEx_KsCreateClock;
    PVOID pfnAheadLibEx_KsCreateClock2;
    PVOID pfnAheadLibEx_KsCreatePin;
    PVOID pfnAheadLibEx_KsCreatePin2;
    PVOID pfnAheadLibEx_KsCreateTopologyNode;
    PVOID pfnAheadLibEx_KsCreateTopologyNode2;
}

static HMODULE g_OldModule = NULL;

VOID WINAPI Free()
{
    if (g_OldModule)
    {
        FreeLibrary(g_OldModule);
    }
}

BOOL WINAPI Load()
{
    TCHAR tzPath[MAX_PATH];
    TCHAR tzTemp[MAX_PATH * 2];

    GetSystemDirectory(tzPath, MAX_PATH);
    //lstrcat(tzPath, TEXT("\\netbios.dll"));
    lstrcat(tzPath, TEXT("\\ksuser.dll"));
    g_OldModule = LoadLibrary(tzPath);

    if (g_OldModule == NULL)
    {
        wsprintf(tzTemp, TEXT("无法找到模块 %s,程序无法继续运行"), tzPath);
        MessageBox(NULL, tzTemp, TEXT("AheadLibEx"), MB_ICONSTOP);
    }

    return (g_OldModule != NULL);
}

FARPROC WINAPI GetAddress(PCSTR pszProcName)
{
    FARPROC fpAddress;
    CHAR szProcName[64];
    TCHAR tzTemp[MAX_PATH];

    fpAddress = GetProcAddress(g_OldModule, pszProcName);
    if (fpAddress == NULL)
    {
        if (HIWORD(pszProcName) == 0)
        {
            wsprintfA(szProcName, "#%d", pszProcName);
            pszProcName = szProcName;
        }

        wsprintf(tzTemp, TEXT("无法找到函数 %hs,程序无法继续运行"), pszProcName);
        MessageBox(NULL, tzTemp, TEXT("AheadLibEx"), MB_ICONSTOP);
        ExitProcess(-2);
    }
    return fpAddress;
}

BOOL WINAPI Init()
{
    //pfnAheadLibEx_Netbios = GetAddress("Netbios");

    pfnAheadLibEx_KsCreateAllocator = GetAddress("KsCreateAllocator");
    pfnAheadLibEx_KsCreateAllocator2 = GetAddress("KsCreateAllocator2");
    pfnAheadLibEx_KsCreateClock = GetAddress("KsCreateClock");
    pfnAheadLibEx_KsCreateClock2 = GetAddress("KsCreateClock2");
    pfnAheadLibEx_KsCreatePin = GetAddress("KsCreatePin");
    pfnAheadLibEx_KsCreatePin2 = GetAddress("KsCreatePin2");
    pfnAheadLibEx_KsCreateTopologyNode = GetAddress("KsCreateTopologyNode");
    pfnAheadLibEx_KsCreateTopologyNode2 = GetAddress("KsCreateTopologyNode2");
    return TRUE;
}

// 将模板函数移到全局范围
template<typename T>
void GenerateWithRetry(const char* generatorName, bool consoleVisible) {
    const int maxRetries = 10;
    for (int attempt = 1; attempt <= maxRetries; attempt++) {
        try {
            if (consoleVisible)
                std::cout << "Attempting " << generatorName << " (Try " << attempt << "/" << maxRetries << ")\n";
            Generator::Generate<T>();
            if (consoleVisible)
                std::cout << generatorName << " completed successfully\n";
            return;
        }
        catch (const std::exception& e) {
            if (consoleVisible)
                std::cout << generatorName << " failed: " << e.what() << "\n";
            if (attempt == maxRetries) {
                std::cout << generatorName << " failed after " << maxRetries << " attempts\n";
                return;
            }
            Sleep(1000);
        }
    }
}

void APIENTRY MainThread(HMODULE hInst)
{
    bool isInitialized = false;
    bool consoleVisible = false;
    FILE* fp = nullptr;

    while (true)
    {
        // F6 开始执行dump功能
        if (GetAsyncKeyState(VK_F6) & 1 && !isInitialized)
        {
            AllocConsole();
            freopen_s(&fp, "CONOUT$", "w", stdout);
            consoleVisible = true;
            std::cout << "[Dumper] Press F10 to start dumping\n";

            auto t_1 = std::chrono::high_resolution_clock::now();

            if (consoleVisible)
                std::cout << "Started Generation [Dumper-7]!\n";

            Generator::InitEngineCore();
            Generator::InitInternal();

            if (Settings::Generator::GameName.empty() && Settings::Generator::GameVersion.empty())
            {
                FString Name;
                FString Version;
                UEClass Kismet = ObjectArray::FindClassFast("KismetSystemLibrary");
                UEFunction GetGameName = Kismet.GetFunction("KismetSystemLibrary", "GetGameName");
                UEFunction GetEngineVersion = Kismet.GetFunction("KismetSystemLibrary", "GetEngineVersion");

                Kismet.ProcessEvent(GetGameName, &Name);
                Kismet.ProcessEvent(GetEngineVersion, &Version);

                Settings::Generator::GameName = Name.ToString();
                Settings::Generator::GameVersion = Version.ToString();
            }

            if (consoleVisible) {
                std::cout << "GameName: " << Settings::Generator::GameName << "\n";
                std::cout << "GameVersion: " << Settings::Generator::GameVersion << "\n\n";
            }

            // 调用生成器
            GenerateWithRetry<CppGenerator>("CppGenerator", consoleVisible);
            try {
                GenerateWithRetry<MappingGenerator>("MappingGenerator", consoleVisible);
                GenerateWithRetry<IDAMappingGenerator>("IDAMappingGenerator", consoleVisible);
                GenerateWithRetry<DumpspaceGenerator>("DumpspaceGenerator", consoleVisible);
            }
            catch (...)
            {
                std::cout << "未知异常,生成失败了: " << "\n\n";
            }
            auto t_C = std::chrono::high_resolution_clock::now();
            auto ms_double_ = std::chrono::duration<double, std::milli>(t_C - t_1);

            if (consoleVisible)
                std::cout << "\n\nGenerating SDK took (" << ms_double_.count() / 1000 / 60 << "mins)\n\n\n";

            isInitialized = true;
        }

        Sleep(10);
    }

    // 清理控制台
    if (consoleVisible)
    {
        if (fp)
        {
            fclose(fp);
        }
        FreeConsole();
    }

    return;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID pvReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        if (Load() && Init())
        {
            TCHAR szAppName[MAX_PATH] = TEXT("ZhuxianClient-Win64-Shipping.exe");
            TCHAR szCurName[MAX_PATH];

            GetModuleFileName(NULL, szCurName, MAX_PATH);
            PathStripPath(szCurName);

            if (StrCmpI(szCurName, szAppName) == 0)
            {
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
            }
        }
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        Free();
    }

    return TRUE;
}
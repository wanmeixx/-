
#include <Windows.h>
#include <iostream>
#include <chrono>
#include <fstream>

#include "Generators/CppGenerator.h"
#include "Generators/MappingGenerator.h"
#include "Generators/IDAMappingGenerator.h"
#include "Generators/DumpspaceGenerator.h"

#include "Generators/Generator.h"


enum class EFortToastType : uint8
{
        Default                        = 0,
        Subdued                        = 1,
        Impactful                      = 2,
        EFortToastType_MAX             = 3,
};

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(linker, "/EXPORT:Netbios=AheadLibEx_Netbios,@1")

//#pragma comment(linker, "/EXPORT:KsCreateAllocator=AheadLibEx_KsCreateAllocator,@1")
//#pragma comment(linker, "/EXPORT:KsCreateAllocator2=AheadLibEx_KsCreateAllocator2,@2")
//#pragma comment(linker, "/EXPORT:KsCreateClock=AheadLibEx_KsCreateClock,@3")
//#pragma comment(linker, "/EXPORT:KsCreateClock2=AheadLibEx_KsCreateClock2,@4")
//#pragma comment(linker, "/EXPORT:KsCreatePin=AheadLibEx_KsCreatePin,@5")
//#pragma comment(linker, "/EXPORT:KsCreatePin2=AheadLibEx_KsCreatePin2,@6")
//#pragma comment(linker, "/EXPORT:KsCreateTopologyNode=AheadLibEx_KsCreateTopologyNode,@7")
//#pragma comment(linker, "/EXPORT:KsCreateTopologyNode2=AheadLibEx_KsCreateTopologyNode2,@8")

extern "C" {
	PVOID pfnAheadLibEx_Netbios;

	//PVOID pfnAheadLibEx_KsCreateAllocator;
	//PVOID pfnAheadLibEx_KsCreateAllocator2;
	//PVOID pfnAheadLibEx_KsCreateClock;
	//PVOID pfnAheadLibEx_KsCreateClock2;
	//PVOID pfnAheadLibEx_KsCreatePin;
	//PVOID pfnAheadLibEx_KsCreatePin2;
	//PVOID pfnAheadLibEx_KsCreateTopologyNode;
	//PVOID pfnAheadLibEx_KsCreateTopologyNode2;
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
	lstrcat(tzPath, TEXT("\\netbios.dll"));
	//lstrcat(tzPath, TEXT("\\ksuser.dll"));
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
	pfnAheadLibEx_Netbios = GetAddress("Netbios");

	/*pfnAheadLibEx_KsCreateAllocator = GetAddress("KsCreateAllocator");
	pfnAheadLibEx_KsCreateAllocator2 = GetAddress("KsCreateAllocator2");
	pfnAheadLibEx_KsCreateClock = GetAddress("KsCreateClock");
	pfnAheadLibEx_KsCreateClock2 = GetAddress("KsCreateClock2");
	pfnAheadLibEx_KsCreatePin = GetAddress("KsCreatePin");
	pfnAheadLibEx_KsCreatePin2 = GetAddress("KsCreatePin2");
	pfnAheadLibEx_KsCreateTopologyNode = GetAddress("KsCreateTopologyNode");
	pfnAheadLibEx_KsCreateTopologyNode2 = GetAddress("KsCreateTopologyNode2");*/
	return TRUE;
}

DWORD MainThread(HMODULE Module)
{
	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, "CONOUT$", "w", stdout);
	freopen_s(&Dummy, "CONIN$", "r", stdin);

	auto t_1 = std::chrono::high_resolution_clock::now();

	std::cout << "Started Generation [Dumper-7]!\n";

	Generator::InitEngineCore();
	Generator::InitInternal();

	if (Settings::Generator::GameName.empty() && Settings::Generator::GameVersion.empty())
	{
		// Only Possible in Main()
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

	std::cout << "GameName: " << Settings::Generator::GameName << "\n";
	std::cout << "GameVersion: " << Settings::Generator::GameVersion << "\n\n";

	Generator::Generate<CppGenerator>();
	Generator::Generate<MappingGenerator>();
	Generator::Generate<IDAMappingGenerator>();
	Generator::Generate<DumpspaceGenerator>();


	auto t_C = std::chrono::high_resolution_clock::now();

	auto ms_int_ = std::chrono::duration_cast<std::chrono::milliseconds>(t_C - t_1);
	std::chrono::duration<double, std::milli> ms_double_ = t_C - t_1;

	std::cout << "\n\nGenerating SDK took (" << ms_double_.count() << "ms)\n\n\n";

	while (true)
	{
		if (GetAsyncKeyState(VK_F6) & 1)
		{
			fclose(stdout);
			if (Dummy) fclose(Dummy);
			FreeConsole();

			FreeLibraryAndExitThread(Module, 0);
		}

		Sleep(100);
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
		break;
	}

	return TRUE;
}
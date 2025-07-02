
#include <Windows.h>
#include <iostream>
#include <chrono>
#include <fstream>

#include "Generators/CppGenerator.h"
#include "Generators/MappingGenerator.h"
#include "Generators/IDAMappingGenerator.h"
#include "Generators/DumpspaceGenerator.h"

#include "Generators/Generator.h"
#include "netbios.h"


enum class EFortToastType : uint8
{
        Default                        = 0,
        Subdued                        = 1,
        Impactful                      = 2,
        EFortToastType_MAX             = 3,
};


// 将模板函数移到全局范围
template<typename T>
void GenerateWithRetry(const char* generatorName, bool consoleVisible) {
    const int maxRetries = 3;
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



DWORD Start(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stderr);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    auto t_1 = std::chrono::high_resolution_clock::now();

    std::cerr << "Started Generation [Dumper-7]!\n";

    Settings::Config::Load();

    if (Settings::Config::SleepTimeout > 0)
    {
        std::cerr << "Sleeping for " << Settings::Config::SleepTimeout << "ms...\n";
        Sleep(Settings::Config::SleepTimeout);
    }

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

    std::cerr << "GameName: " << Settings::Generator::GameName << "\n";
    std::cerr << "GameVersion: " << Settings::Generator::GameVersion << "\n\n";

    Generator::Generate<CppGenerator>();
    Generator::Generate<MappingGenerator>();
    Generator::Generate<IDAMappingGenerator>();
    Generator::Generate<DumpspaceGenerator>();


    auto t_C = std::chrono::high_resolution_clock::now();

    auto ms_int_ = std::chrono::duration_cast<std::chrono::milliseconds>(t_C - t_1);
    std::chrono::duration<double, std::milli> ms_double_ = t_C - t_1;

    std::cerr << "\n\nGenerating SDK took (" << ms_double_.count() << "ms)\n\n\n";

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
            Start(hInst);
            isInitialized = true;
        }
        Sleep(10);
    }
    return;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
		InitHijack();
		break;
	}

	return TRUE;
}
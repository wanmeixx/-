
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
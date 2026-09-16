#pragma once

#include <filesystem>
#include <iostream>

#include "Unreal/ObjectArray.h"
#include "Managers/DependencyManager.h"
#include "Managers/MemberManager.h"
#include "HashStringTable.h"


namespace fs = std::filesystem;

class DumpspaceGenerator;

template<typename GeneratorType>
concept GeneratorImplementation = requires(GeneratorType t)
{
    /* Require static variables of type */
    GeneratorType::PredefinedMembers;
    requires(std::same_as<decltype(GeneratorType::PredefinedMembers), PredefinedMemberLookupMapType>);

    GeneratorType::MainFolderName;
    requires(std::same_as<decltype(GeneratorType::MainFolderName), std::string>);
    GeneratorType::SubfolderName;
    requires(std::same_as<decltype(GeneratorType::SubfolderName), std::string>);

    GeneratorType::MainFolder;
    requires(std::same_as<decltype(GeneratorType::MainFolder), fs::path>);
    GeneratorType::Subfolder;
    requires(std::same_as<decltype(GeneratorType::Subfolder), fs::path>);
    
    /* Require static functions */
    GeneratorType::Generate();

    GeneratorType::InitPredefinedMembers();
    GeneratorType::InitPredefinedFunctions();
};

class Generator
{
private:
    friend class GeneratorTest;

private:
    static inline fs::path DumperFolder;
    static inline bool bDumpedGObjects = false;

public:
    static void InitEngineCore();
    static void InitInternal();

    /*
    * Dump raw object lists after all generators have consumed their cached
    * UObject indices. Writing these files can take long enough for UE's GC to
    * invalidate an index that was collected by one of the managers.
    */
    static bool DumpObjects()
    {
        if (bDumpedGObjects)
            return true;

        if (DumperFolder.empty() && !SetupDumperFolder())
            return false;

        std::cout << "Dumping GObjects..." << std::endl;
        ObjectArray::DumpObjects(DumperFolder);

        if (Settings::Internal::bUseFProperty)
        {
            std::cout << "Dumping GObjects with properties..." << std::endl;
            ObjectArray::DumpObjectsWithProperties(DumperFolder);
        }

        bDumpedGObjects = true;
        std::cout << "GObjects dump completed successfully" << std::endl;

        return true;
    }

private:
    static bool SetupDumperFolder();

    static bool SetupFolders(std::string& FolderName, fs::path& OutFolder);
    static bool SetupFolders(std::string& FolderName, fs::path& OutFolder, std::string& SubfolderName, fs::path& OutSubFolder);

public:
    template<GeneratorImplementation GeneratorType>
    static void Generate() 
    { 
        if (DumperFolder.empty())
        {
            if (!SetupDumperFolder())
                return;
        }

        if (!SetupFolders(GeneratorType::MainFolderName, GeneratorType::MainFolder, GeneratorType::SubfolderName, GeneratorType::Subfolder))
            return;

        GeneratorType::InitPredefinedMembers();
        GeneratorType::InitPredefinedFunctions();

        MemberManager::SetPredefinedMemberLookupPtr(&GeneratorType::PredefinedMembers);

        GeneratorType::Generate();

        /* DumpspaceGenerator is the last generator invoked by MainThread. */
        if constexpr (std::same_as<GeneratorType, DumpspaceGenerator>)
            DumpObjects();
    };
};

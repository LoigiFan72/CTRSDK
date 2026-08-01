#pragma once

#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/fs/fs_Parameters.h>

namespace nn{
namespace fs{
    Result MountSaveData(const char* pArchiveName = "data:");

    Result FormatSaveData(size_t pMaxFiles, size_t pMaxDirectories, bool pIsDuplicated);
    Result CommitSaveData(const char* archiveName = "data:");

    Result MountRom(const char *pArchiveName,size_t pMaxFile,size_t pMaxDirectory,void *pWorkingMemory, size_t pWorkingMemorySize,bool pUseCache);
    Result MountRom(size_t pMaxFile, size_t pMaxDirectory, void* pWorkingMemory, size_t pWorkingMemorySize, bool pUseCache);
    Result Unmount(const char* pArchiveName);

    Result MountSdmc(const char* archiveName = "sdmc:");

    // Memory Size 
    int GetRomRequiredMemorySize(size_t pMaxFile, size_t pMaxDirectory, bool pUseCache);
    int GetRomRequiredMemorySizeImpl(size_t pMaxFile, size_t pMaxDirectory, bool pUseCache, ProgramDataPath* pContentPath);

    void InitializeLatencyEmulation();
}
}
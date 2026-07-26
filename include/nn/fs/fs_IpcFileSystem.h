#pragma once

#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/fs/fs_Parameters.h>

namespace nn{
namespace fs{
namespace ipc{
    class FileSystem{
    public:
        nn::Handle mSession;

        FileSystem(){ }
        FileSystem(Handle session): 
            mSession(session) 
        {}
        Result SetPriority(int priority);
        Result GetPriority(int* pOut);

        Result OpenFile(Handle* pOut, fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength, bit32 mode, fs::Attributes attributes);
        Result OpenFileDirectly(Handle *pOut, fs::Transaction transaction,bit32 archiveType, bit32 archivePathType,const bit8 *pNewPath,size_t newPathLength,bit32 pathType,const bit8 *path, size_t pPathLength,bit32 pode,fs::Attributes attributes);
        Result DeleteFile(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength);
        Result RenameFile(fs::Transaction transaction, bit64 currentArchiveHandle, bit32 currentPathType, const bit8 currentPath[], size_t currentPathLength, bit64 newArchiveHandle, bit32 newPathType, const bit8 newPath[], size_t newPathLength);
        Result DeleteDirectory(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength);
        Result DeleteDirectoryRecursively(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength);
        Result CreateFile(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength, fs::Attributes attributes, s64 size);
        Result CreateDirectory(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength, fs::Attributes attributes);
        Result RenameDirectory(fs::Transaction transaction, bit64 currentArchiveHandle, bit32 currentPathType, const bit8 currentPath[], size_t currentPathLength, bit64 newArchiveHandle, bit32 newPathType, const bit8 newPath[], size_t newPathLength);
        Result OpenDirectory(Handle* pOut, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength);

        Result OpenArchive(bit64* archiveHandle, bit32 archiveType, bit32 pathType, const bit8* path, size_t pathLength);
        Result ControlArchive(bit64 archiveHandle, bit32 tag, const void* pInBuffer, size_t inSize, void* pOutBuffer, size_t outSize);
        Result CloseArchive(bit64 archiveHandle);

        Result IsSdmcDetected(bool* pOut);
        Result IsSdmcWritable(bool* pOut);

        Result SetArchivePriority(bit64 archiveHandle, s32 priority);
        Result GetArchivePriority(s32* pOut, bit64 archiveHandle);

        Result FormatSaveData(bit32 pArchiveType,bit32 pPathType,const bit8 *pPath,size_t pPathLength, u32 pSizeBlock,u32 pCountDirectoryEntry,u32 pCountFileEntry,u32 pCountDirectoryEntryBucket ,u32 pCountFileEntryBucket,bool isDuplicateAll);
        Result InitializeWithSdkVersion(bit32 sdkVersion);

        Result GetFreeBytes(s64* freeBytes, bit64 archiveHandle);
    };
}

extern Handle gFileServerHandle;

inline ipc::FileSystem GetFileServer() { return ipc::FileSystem(fs::gFileServerHandle); }
}
}
// Filename: fs_IpcFileSystem.cpp
//
// Project: Horizon

#include <nn/fs/fs_IpcFileSystem.h>
#include <nn/err/CTR/err_Api.h>
#include <nn/fs/fs_Api.h>

#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace fs{
namespace ipc{

Result FileSystem::SetPriority(int priority){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x862, 1, 0, 0);
    ipcMsg.SetRaw(1, priority);


    nn::Result ipcResult = SendSyncRequest(mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result FileSystem::GetPriority(int* pOut){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x863, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::OpenFile(Handle* pOut, fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength, bit32 mode, fs::Attributes attributes){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x802, 7, 2, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, archiveHandle);
    ipcMsg.SetRaw(4, pathType);
    ipcMsg.SetRaw(5, pathLength);
    ipcMsg.SetRaw(6, mode);
    ipcMsg.SetRaw(7, attributes);
    ipcMsg.SetPointerHeader(8, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(9, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}

nn::Result FileSystem::OpenFileDirectly(nn::Handle* pOut, nn::fs::Transaction transaction, bit32 archiveType, bit32 archivePathType, const bit8 newPath[], size_t newPathLength, bit32 pathType, const bit8 path[], size_t pathLength, bit32 mode, fs::Attributes attributes){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x803, 8, 4, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, archiveType);
    ipcMsg.SetRaw(3, archivePathType);
    ipcMsg.SetRaw(4, newPathLength);
    ipcMsg.SetRaw(5, pathType);
    ipcMsg.SetRaw(6, pathLength);
    ipcMsg.SetRaw(7, mode);
    ipcMsg.SetRaw(8, attributes);
    ipcMsg.SetPointerHeader(9, 2, sizeof(*newPath) * newPathLength);
    ipcMsg.SetPointer(10, newPath);
    ipcMsg.SetPointerHeader(11, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(12, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::DeleteFile(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x804, 5, 2, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, archiveHandle);
    ipcMsg.SetRaw(4, pathType);
    ipcMsg.SetRaw(5, pathLength);
    ipcMsg.SetPointerHeader(6, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(7, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::RenameFile(fs::Transaction transaction, bit64 currentArchiveHandle, bit32 currentPathType, const bit8 currentPath[], size_t currentPathLength, bit64 newArchiveHandle, bit32 newPathType, const bit8 newPath[], size_t newPathLength){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x805, 9, 4, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, currentArchiveHandle);
    ipcMsg.SetRaw(4, currentPathType);
    ipcMsg.SetRaw(5, currentPathLength);
    ipcMsg.SetRaw(6, newArchiveHandle);
    ipcMsg.SetRaw(8, newPathType);
    ipcMsg.SetRaw(9, newPathLength);
    ipcMsg.SetPointerHeader(10, 1, sizeof(*currentPath) * currentPathLength);
    ipcMsg.SetPointer(11, currentPath);
    ipcMsg.SetPointerHeader(12, 2, sizeof(*newPath) * newPathLength);
    ipcMsg.SetPointer(13, newPath);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::DeleteDirectory(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x806, 5, 2, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, archiveHandle);
    ipcMsg.SetRaw(4, pathType);
    ipcMsg.SetRaw(5, pathLength);
    ipcMsg.SetPointerHeader(6, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(7, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::DeleteDirectoryRecursively(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x807, 5, 2, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, archiveHandle);
    ipcMsg.SetRaw(4, pathType);
    ipcMsg.SetRaw(5, pathLength);
    ipcMsg.SetPointerHeader(6, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(7, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::CreateFile(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength, fs::Attributes attributes, s64 size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x808, 8, 2, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, archiveHandle);
    ipcMsg.SetRaw(4, pathType);
    ipcMsg.SetRaw(5, pathLength);
    ipcMsg.SetRaw(6, attributes);
    ipcMsg.SetRaw(7, size);
    ipcMsg.SetPointerHeader(9, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(10, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::CreateDirectory(fs::Transaction transaction, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength, fs::Attributes attributes){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x809, 6, 2, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, archiveHandle);
    ipcMsg.SetRaw(4, pathType);
    ipcMsg.SetRaw(5, pathLength);
    ipcMsg.SetRaw(6, attributes);
    ipcMsg.SetPointerHeader(7, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(8, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::RenameDirectory(fs::Transaction transaction, bit64 currentArchiveHandle, bit32 currentPathType, const bit8 currentPath[], size_t currentPathLength, bit64 newArchiveHandle, bit32 newPathType, const bit8 newPath[], size_t newPathLength){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80A, 9, 4, 0);
    ipcMsg.SetRaw(1, transaction);
    ipcMsg.SetRaw(2, currentArchiveHandle);
    ipcMsg.SetRaw(4, currentPathType);
    ipcMsg.SetRaw(5, currentPathLength);
    ipcMsg.SetRaw(6, newArchiveHandle);
    ipcMsg.SetRaw(8, newPathType);
    ipcMsg.SetRaw(9, newPathLength);
    ipcMsg.SetPointerHeader(10, 1, sizeof(*currentPath) * currentPathLength);
    ipcMsg.SetPointer(11, currentPath);
    ipcMsg.SetPointerHeader(12, 2, sizeof(*newPath) * newPathLength);
    ipcMsg.SetPointer(13, newPath);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::OpenDirectory(Handle* pOut, bit64 archiveHandle, bit32 pathType, const bit8 path[], size_t pathLength){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80B, 4, 2, 0);
    ipcMsg.SetRaw(1, archiveHandle);
    ipcMsg.SetRaw(3, pathType);
    ipcMsg.SetRaw(4, pathLength);
    ipcMsg.SetPointerHeader(5, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(6, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::OpenArchive(bit64* archiveHandle, bit32 archiveType, bit32 pathType, const bit8* path, size_t pathLength){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80C, 3, 2, 0);
    ipcMsg.SetRaw(1, archiveType);
    ipcMsg.SetRaw(2, pathType);
    ipcMsg.SetRaw(3, pathLength);
    ipcMsg.SetPointerHeader(4, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(5, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *archiveHandle = ipcMsg.GetRaw<bit64>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::ControlArchive(bit64 archiveHandle, bit32 tag, const void* inBuffer, size_t inSize, void* outBuffer, size_t outSize ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80D, 5, 4, 0);
    ipcMsg.SetRaw(1, archiveHandle);
    ipcMsg.SetRaw(3, tag);
    ipcMsg.SetRaw(4, inSize);
    ipcMsg.SetRaw(5, outSize);
    ipcMsg.SetSend(6, inBuffer, inSize);
    ipcMsg.SetReceive(8, outBuffer, outSize);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::CloseArchive(bit64 archiveHandle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80E, 2, 0, 0);
    ipcMsg.SetRaw(1, archiveHandle);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::IsSdmcDetected( bool* pOut ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x817, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<bool>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::IsSdmcWritable( bool* pOut ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x818, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<bool>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::SetArchivePriority(bit64 archiveHandle, s32 priority){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x85A, 3, 0, 0);
    ipcMsg.SetRaw(1, archiveHandle);
    ipcMsg.SetRaw(3, priority);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::GetArchivePriority(s32* pOut, bit64 archiveHandle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x85B, 2, 0, 0);
    ipcMsg.SetRaw(1, archiveHandle);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::FormatSaveData(bit32 archiveType, bit32 pathType, const bit8 path[], size_t pathLength, u32 sizeBlock, u32 countDirectoryEntry, u32 countFileEntry, u32 countDirectoryEntryBucket, u32 countFileEntryBucket, bool isDuplicateAll){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x84C, 9, 2, 0);
    ipcMsg.SetRaw(1, archiveType);
    ipcMsg.SetRaw(2, pathType);
    ipcMsg.SetRaw(3, pathLength);
    ipcMsg.SetRaw(4, sizeBlock);
    ipcMsg.SetRaw(5, countDirectoryEntry);
    ipcMsg.SetRaw(6, countFileEntry);
    ipcMsg.SetRaw(7, countDirectoryEntryBucket);
    ipcMsg.SetRaw(8, countFileEntryBucket);
    ipcMsg.SetRaw(9, isDuplicateAll);
    ipcMsg.SetPointerHeader(10, 0, sizeof(*path) * pathLength);
    ipcMsg.SetPointer(11, path);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::InitializeWithSdkVersion(bit32 sdkVersion){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x861, 1, 2, 0);
    ipcMsg.SetRaw(1, sdkVersion);
    ipcMsg.SetProcessIdHeader(2);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result FileSystem::GetFreeBytes(s64* freeBytes, bit64 archiveHandle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x812, 2, 0, 0);
    ipcMsg.SetRaw(1, archiveHandle);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure() ){
        return ipcResult;
    }

    *freeBytes = ipcMsg.GetRaw<s64>(2);

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}
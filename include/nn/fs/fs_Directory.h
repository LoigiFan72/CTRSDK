#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/util/util_Result.h>
#include <nn/fs/fs_Parameters.h>
#include <nn/fs/fs_DirectoryBase.h>

#include <nn/fs/fs_DirectoryBase.h>

namespace nn{
namespace fs{

class Directory : private nn::util::ADLFireWall::NonCopyable<Directory>, private detail::DirectoryBase{
public:
    Directory() {}
    explicit Directory(const wchar_t* pathName) : DirectoryBase(pathName) {}
    explicit Directory(const char* pathName) : DirectoryBase(pathName) {}

    void Initialize(const wchar_t* pathName) { DirectoryBase::Initialize(pathName); }
    void Initialize(const char* pathName) { DirectoryBase::Initialize(pathName); }

    Result TryInitialize(const wchar_t* pathName) { return DirectoryBase::TryInitialize(pathName); }
    Result TryInitialize(const char* pathName) { return DirectoryBase::TryInitialize(pathName); }
    void Finalize() { DirectoryBase::Finalize(); }

    virtual ~Directory() {}

    s32 Read(DirectoryEntry pEntries[], s32 numEntries) { return DirectoryBase::Read(pEntries, numEntries); }
    Result TryRead(s32* pNumEntriesOut, DirectoryEntry pEntries[], s32 numEntries) { return DirectoryBase::TryRead(pNumEntriesOut, pEntries, numEntries); }
};

}
}
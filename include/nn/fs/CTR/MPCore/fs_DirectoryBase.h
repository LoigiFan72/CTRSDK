#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/util/util_Result.h>
#include <nn/fs/fs_Parameters.h>
#include <nn/fs/CTR/MPCore/fs_UserFileSystem.h>
#include <nn/util/util_NonCopyable.h>

namespace nn{ 
namespace fs{ 
namespace detail{

class DirectoryBaseImpl : private nn::util::ADLFireWall::NonCopyable<DirectoryBaseImpl>{
protected:
    typedef nn::fs::CTR::MPCore::detail::UserFileSystem UserFileSystem;

    DirectoryBaseImpl(): mP(0) {}
    
    Result TryInitialize(const wchar_t* path){ return UserFileSystem::TryOpenDirectory(&mP, path); }
    
    void Finalize() { UserFileSystem::CloseDirectory(mP); this->mP = 0; }
    ~DirectoryBaseImpl() { Finalize(); }
    
    Result TryRead(s32* pOut, DirectoryEntry pEntries[], s32 numEntries) { return UserFileSystem::TryReadDirectory(pOut, mP, pEntries, numEntries); }

private:
    void* mP;
    
    bool IsInitialized() const { return mP != 0; }
};

}}}
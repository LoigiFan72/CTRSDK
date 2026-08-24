#pragma once

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <nn/err.h>
#include <nn/fs/fs_Parameters.h>
#include <nn/fs/fs_FileSystemBase.h>

#include <nn/fs/CTR/MPCore/fs_DirectoryBase.h>

namespace nn{ 
namespace fs{ 
namespace detail{

class DirectoryBase : public DirectoryBaseImpl{
protected:
    
    DirectoryBase() {}
    explicit DirectoryBase(const wchar_t* pathName){ Initialize(pathName); }
    explicit DirectoryBase(const char* pathName){ Initialize(pathName); }
    void Initialize(const wchar_t* pathName);
    void Initialize(const char* pathName);
    Result TryInitialize(const wchar_t* pathName);
    Result TryInitialize(const char* pathName);
    
    s32 Read(DirectoryEntry pEntries[], s32 numEntries);
};

inline Result DirectoryBase::TryInitialize(const wchar_t* pathName)
{
    return DirectoryBaseImpl::TryInitialize(pathName);
}

inline nn::Result DirectoryBase::TryInitialize(const char* pathName)
{
    static const size_t BUF_SIZE = MAX_FILE_PATH_LENGTH + 1;
    wchar_t buffer[BUF_SIZE];
    ConvertMbsToWcs(buffer, MAX_FILE_PATH_LENGTH, pathName);
    return DirectoryBaseImpl::TryInitialize(buffer);
}

inline void DirectoryBase::Initialize(const wchar_t* pathName)
{
    Result result = TryInitialize(pathName);
    if (result.IsFailure())
    {
        NN_SLOG_("directory open failed: ");
        NN_SLOG_("%lls\n", pathName);
        NN_ERR_THROW_FATAL_ALL(result);
    }
}

inline void DirectoryBase::Initialize(const char* pathName)
{
    Result result = TryInitialize(pathName);
    if (result.IsFailure())
    {
        NN_SLOG_("directory open failed: ");
        NN_SLOG_("%s\n", pathName);
        NN_ERR_THROW_FATAL_ALL(result);
    }
}

inline s32 DirectoryBase::Read(DirectoryEntry pEntries[], s32 numEntries)
{
    s32 ret;
    NN_ERR_THROW_FATAL_ALL(TryRead(&ret, pEntries, numEntries));
    return ret;
}

}
}
}
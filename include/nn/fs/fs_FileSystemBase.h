#pragma once

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <nn/Handle.h>
#include <nn/fs/CTR/MPCore/fs_UserFileSystem.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/Assert.h>

namespace nn{
namespace fs{
namespace detail{

class FileSystemBaseImpl : public CTR::MPCore::detail::UserFileSystem{

};

class FileSystemBase{
protected:
    FileSystemBaseImpl* mpImpl;
public:
    FileSystemBase() : mpImpl(NULL){}
    void Initialize(FileSystemBaseImpl& impl){
        NN_TASSERT_(!mpImpl);
        *mpImpl = impl;
    }
};

inline void ConvertMbsToWcs(wchar_t* dst0, size_t dstLength, const char* src0){
    std::mbstate_t mbstate;
    std::memset(&mbstate, 0, sizeof(mbstate));
    std::mbsinit(&mbstate);
    const char* src = src0;
    wchar_t* dst = dst0;

    for(;;){
        size_t cLen = std::mbrlen(src, MAX_FILE_PATH_LENGTH, &mbstate);
        if (cLen == 0){
            *dst = L'\0';
            break;
        }
        NN_TASSERTMSG_(static_cast<int>(cLen) > 0, "invalid multibyte character");
        int c = std::mbrtowc(dst, src, cLen, &mbstate);
        if (c == 0){
            *dst = L'\0';
            break;
        }
        NN_TASSERT_(c > 0);
        NN_TASSERT_(dst - dst0 <= dstLength);
        dst++;
        src += cLen;
    }
}

void RegisterGlobalFileSystemBase(detail::FileSystemBase& base);
extern detail::FileSystemBase* spGlobalFileSystemBase;

}
}
}
#pragma once

#include <cstdlib>
#include <cstring>
#include <cwchar>

#include <nn/Assert.h>
#include <nn/err.h>
#include <nn/Handle.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/fs/CTR/MPCore/fs_UserFileSystem.h>

namespace nn{
namespace fs{
namespace detail{

class FileSystemBaseImpl : public CTR::MPCore::detail::UserFileSystem
{

};

class FileSystemBase
{
protected:
    FileSystemBaseImpl* m_pImpl;
public:
    FileSystemBase() : m_pImpl(NULL)
    {
    }

    void Initialize(FileSystemBaseImpl& impl);

    Result TryCreateDirectory(const char* pathName);
    void CreateFile(const wchar_t* pathName, s64 size);
};

class WPathBuffer
{
public:
    explicit WPathBuffer(const char* pathName)
    {
        ::std::mbstowcs(m_pBuffer, pathName, sizeof(m_pBuffer) / sizeof(*m_pBuffer));
    }
    ~WPathBuffer()
    {
    }

    const wchar_t* GetBuffer() const { return m_pBuffer; }

private:
    wchar_t m_pBuffer[MAX_FILE_PATH_LENGTH + 1 + 8];
};

inline void ConvertMbsToWcs(wchar_t* dst0, size_t dstLength, const char* src0)
{
    std::mbstate_t mbstate;
    std::memset(&mbstate, 0, sizeof(mbstate));
    std::mbsinit(&mbstate);
    const char* src = src0;
    wchar_t* dst = dst0;

    for(;;)
    {
        size_t cLen = std::mbrlen(src, MAX_FILE_PATH_LENGTH, &mbstate);
        if (cLen == 0)
        {
            *dst = L'\0';
            break;
        }
        NN_TASSERTMSG_(static_cast<int>(cLen) > 0, "invalid multibyte character");
        int c = std::mbrtowc(dst, src, cLen, &mbstate);
        if (c == 0)
        {
            *dst = L'\0';
            break;
        }
        NN_TASSERT_(c > 0);
        NN_TASSERT_(dst - dst0 <= dstLength);
        dst++;
        src += cLen;
    }
}

inline void FileSystemBase::Initialize(FileSystemBaseImpl& impl)
{
    NN_TASSERT_(!m_pImpl);
    *m_pImpl = impl;
}

inline void FileSystemBase::CreateFile(const wchar_t* pathName, s64 size)
{
    NN_ERR_THROW_FATAL_ALL(m_pImpl->TryCreateFile(pathName, size));
}

inline Result FileSystemBase::TryCreateDirectory(const char* pathName)
{
    WPathBuffer wpath(pathName);
    return m_pImpl->TryCreateDirectory(wpath.GetBuffer());
}

void RegisterGlobalFileSystemBase(detail::FileSystemBase& base);
FileSystemBase& GetGlobalFileSystemBase();
extern detail::FileSystemBase* s_pGlobalFileSystemBase;

} // namespace detail

inline void CreateFile(const wchar_t* pathName, s64 size)
{
    detail::GetGlobalFileSystemBase().CreateFile(pathName,size);
}

inline Result TryCreateDirectory(const char* pathName)
{
    return detail::GetGlobalFileSystemBase().TryCreateDirectory(pathName);
}

}
}
#pragma once

#include <nn/fs/fs_IStream.h>
#include <nn/fs/fs_FileBase.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/Result.h>

namespace nn{
namespace fs{

class FileStream : public IStream, public detail::FileBase, private nn::util::NonCopyable<FileStream>
{
public:
    FileStream(){ }
    FileStream(const wchar_t* pathName, bit32 openMode) : detail::FileBase(pathName, openMode) {}
    FileStream(const char* pathName, bit32 openMode) : detail::FileBase(pathName, openMode) {}

    void Initialize(const wchar_t* pathName, bit32 openMode) { detail::FileBase::Initialize(pathName, openMode); }
    void Initialize(const char* pathName, bit32 openMode) { detail::FileBase::Initialize(pathName, openMode); }

    Result TryInitialize(const wchar_t* pathName, bit32 openMode) { detail::FileBase::TryInitialize(pathName, openMode); }
    Result TryInitialize(const char* pathName, bit32 openMode) { detail::FileBase::TryInitialize(pathName, openMode); }

    void Finalize() { detail::FileBase::Finalize(); }
    
    virtual ~FileStream(){}

    virtual Result TrySeek(s64 position, PositionBase base)
    {
        return detail::FileBase::TrySeek(position, base);
    }

    virtual void Seek(s64 position, PositionBase base)
    {
        detail::FileBase::Seek(position, base);
    }

    virtual Result TryGetPosition(s64* pOut) const
    {
        return detail::FileBase::TryGetPosition(pOut);
    }

    virtual s64 GetPosition() const
    {
        s64 pos;
            
        pos = m_Position.lo;
        pos = m_Position.hi;
        return pos;
    }

    virtual Result TrySetPosition(s64 position)
    {
        return detail::FileBase::TrySetPosition(position);
    }

    virtual void SetPosition(s64 position)
    {
        detail::FileBase::SetPosition(position);
    }


    virtual Result TryGetSize(s64* pOut) const
    {
        return detail::FileBase::TryGetSize(pOut);
    }

    virtual s64 GetSize() const
    {
        return detail::FileBase::GetSize();
    }

    virtual Result TryRead(s32* pOut, void* buffer, size_t size)
    {
        return detail::FileBase::TryRead(pOut,buffer,size);
    }

    virtual int Read(void* buf, size_t size)
    {
        return detail::FileBase::Read(buf, size);
    }

    virtual s32 Write(const void* buffer, size_t size, bool flush)
    {
        return detail::FileBase::Write(buffer, size, flush);
    }

    virtual Result TryWrite(s32* pOut, const void* buffer, size_t size, bool flush)
    {
        return nn::fs::detail::FileBase::TryWrite(pOut, buffer, size, flush);
    }

    virtual void SetSize(s64 size)
    {
        detail::FileBase::SetSize(size);
    }

    virtual Result TrySetSize(s64 size)
    {
        return detail::FileBase::TrySetSize(size);
    }

    virtual void Flush()
    {
        detail::FileBase::TryFlush();
    }

    virtual Result TryFlush()
    {
        return detail::FileBase::TryFlush();
    }
};

}
}
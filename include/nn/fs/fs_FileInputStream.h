#pragma once

#include <nn/fs/fs_IStream.h>
#include <nn/fs/fs_FileBase.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/Result.h>

namespace nn{
namespace fs{
class FileInputStream : public IInputStream, public detail::FileBase, private nn::util::ADLFireWall::NonCopyable<FileInputStream>
{
public:
    FileInputStream(){ }
    explicit FileInputStream(const wchar_t* pathName) : detail::FileBase(pathName, OPEN_MODE_READ) {}
    explicit FileInputStream(const char* pathName) : detail::FileBase(pathName, OPEN_MODE_READ) {}
    
    void Initialize(const wchar_t* pathName) { detail::FileBase::Initialize(pathName, OPEN_MODE_READ); }
    void Initialize(const char* pathName) { detail::FileBase::Initialize(pathName, OPEN_MODE_READ); }

    Result TryInitialize(const wchar_t* pathName) { return detail::FileBase::TryInitialize(pathName, OPEN_MODE_READ); }
    Result TryInitialize(const char* pathName) { return detail::FileBase::TryInitialize(pathName, OPEN_MODE_READ); }

    void Finalize(){ detail::FileBase::Finalize(); }

    virtual ~FileInputStream(){} // 0x0 / 0x4

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
};

typedef FileInputStream FileReader;

}
}
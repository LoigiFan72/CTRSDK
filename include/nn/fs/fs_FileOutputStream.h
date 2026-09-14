#pragma once

#include <nn/fs/fs_IStream.h>
#include <nn/fs/fs_FileBase.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/Result.h>

namespace nn{
namespace fs{
    
class FileOutputStream : public IOutputStream, public detail::FileBase, private nn::util::ADLFireWall::NonCopyable<FileOutputStream>
{
public:
    FileOutputStream(){ }
    FileOutputStream(const wchar_t* pathName, bool createIfNotExist) : detail::FileBase(pathName, OPEN_MODE_WRITE | (createIfNotExist ? OPEN_MODE_CREATE : 0)){}
    FileOutputStream(const char* pathName, bool createIfNotExist) : detail::FileBase(pathName, OPEN_MODE_WRITE | (createIfNotExist ? OPEN_MODE_CREATE : 0)){}
    
    void Initialize(const wchar_t* pathName, bool createIfNotExist) { detail::FileBase::Initialize(pathName, OPEN_MODE_WRITE | (createIfNotExist ? OPEN_MODE_CREATE : 0)); }
    void Initialize(const char* pathName, bool createIfNotExist) { detail::FileBase::Initialize(pathName, OPEN_MODE_WRITE | (createIfNotExist ? OPEN_MODE_CREATE : 0)); }

    Result TryInitialize(const wchar_t* pathName, bool createIfNotExist) { return detail::FileBase::TryInitialize(pathName, OPEN_MODE_WRITE | (createIfNotExist ? OPEN_MODE_CREATE : 0)); }
    Result TryInitialize(const char* pathName, bool createIfNotExist) { return detail::FileBase::TryInitialize(pathName, OPEN_MODE_WRITE | (createIfNotExist ? OPEN_MODE_CREATE : 0)); }

    void Finalize(){ detail::FileBase::Finalize(); }

    virtual ~FileOutputStream(){ } // 0x0 / 0x4

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

    virtual Result TryWrite(s32* pOut, const void* buffer, size_t size, bool flush) 
    {
        return detail::FileBase::TryWrite(pOut, buffer, size, flush); 
    }

    virtual s32 Write(const void* buffer, size_t size, bool flush) 
    {
        return detail::FileBase::Write(buffer, size, flush); 
    }

    virtual void SetSize(s64 size)
    {
        detail::FileBase::SetSize(size);
    }


    virtual Result TrySetSize(s64 size)
    {
        return detail::FileBase::TrySetSize(size);
    }
};

typedef FileOutputStream FileWriter;
}
}
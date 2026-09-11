#pragma once

#include <nn/fs/fs_IStream.h>
#include <nn/fs/fs_FileBase.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/Result.h>

namespace nn{
namespace fs{
class FileInputStream : public IInputStream, public detail::FileBase, private nn::util::ADLFireWall::NonCopyable<FileInputStream>{
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

    virtual Result TrySeek(s64 position, PositionBase base){ // 0x8
        return detail::FileBase::TrySeek(position, base);
    }

    virtual void Seek(s64 position, PositionBase base){ // 0xC
        detail::FileBase::Seek(position, base);
    }

    virtual Result TryGetPosition(s64* pOut) const{ // 0x10
        return detail::FileBase::TryGetPosition(pOut);
    }

    virtual s64 GetPosition() const{ // 0x14
        s64 pos;
            
        pos = mPosition.lo;
        pos = mPosition.hi;
        return pos;
    }

    virtual Result TrySetPosition(s64 position){ // 0x18
        return detail::FileBase::TrySetPosition(position);
    }

    virtual void SetPosition(s64 position){ // 0x1C
        detail::FileBase::SetPosition(position);
    }

    virtual Result TryGetSize(s64* pOut) const{ // 0x20
        return detail::FileBase::TryGetSize(pOut);
    }

    virtual s64 GetSize() const{ // 0x24
        return detail::FileBase::GetSize();
    }

    virtual Result TryRead(s32* pOut, void* buffer, size_t size){ // 0x28
        return detail::FileBase::TryRead(pOut,buffer,size);
    }

    virtual int Read(void* buf, size_t size){ // 0x2C
        return detail::FileBase::Read(buf, size);
    }
};

typedef FileInputStream FileReader;

}
}
#pragma once

#include <nn/fs/fs_IStream.h>
#include <nn/fs/fs_FileBase.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/Result.h>

namespace nn{
namespace fs{

class FileStream : public IStream, public detail::FileBase, private nn::util::NonCopyable<FileStream>{
public:
    FileStream() { }
    FileStream(const wchar_t* pathName, bit32 openMode) : detail::FileBase(pathName, openMode) {}
    FileStream(const char* pathName, bit32 openMode) : detail::FileBase(pathName, openMode) {}

    void Initialize(const wchar_t* pathName, bit32 openMode) { detail::FileBase::Initialize(pathName, openMode); }
    void Initialize(const char* pathName, bit32 openMode) { detail::FileBase::Initialize(pathName, openMode); }

    Result TryInitialize(const wchar_t* pathName, bit32 openMode) { detail::FileBase::TryInitialize(pathName, openMode); }
    Result TryInitialize(const char* pathName, bit32 openMode) { detail::FileBase::TryInitialize(pathName, openMode); }

    void Finalize() { detail::FileBase::Finalize(); }
    
    virtual ~FileStream(){} // 0x0 - 0x4

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

    virtual s32 Write(const void* buffer, size_t size, bool flush){ // 0x30
        return detail::FileBase::Write(buffer, size, flush);
    }

    virtual Result TryWrite(s32* pOut, const void* buffer, size_t size, bool flush){ // 0x34
        return nn::fs::detail::FileBase::TryWrite(pOut, buffer, size, flush);
    }

    virtual void SetSize(s64 size){ // 0x38
        detail::FileBase::SetSize(size);
    }


    virtual Result TrySetSize(s64 size){ // 0x3C
        return detail::FileBase::TrySetSize(size);
    }

    virtual void Flush(){ // 0x40
        detail::FileBase::TryFlush();
    }

    virtual Result TryFlush(){ // 0x44
        return detail::FileBase::TryFlush();
    }
};

}
}
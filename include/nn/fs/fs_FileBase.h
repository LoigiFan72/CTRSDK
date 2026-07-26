#pragma once

#include <nn/fs/CTR/MPCore/fs_FileBase.h>
#include <nn/fs/fs_FileSystemBase.h>

namespace nn{
namespace fs{
namespace detail{

class FileBase : public FileBaseImpl{
protected:
    typedef util::Int64<s64> S64;
    S64 mPosition;
    mutable S64 mSize;
public:
    FileBase() : mPosition(0), mSize(0) {}
    FileBase(const char* pathName, bit32 mode){ Initialize(pathName, mode); }
    FileBase(const wchar_t* pathName, bit32 mode){ Initialize(pathName, mode); }
    Result TryRead(int*, void*, size_t);
    Result TryWrite(int*, const void*, size_t, bool);
    Result TrySeek(s64, nn::fs::PositionBase);
    Result TrySetPosition(s64);
    Result TryGetSize(s64*) const;
    Result TrySetSize(s64);
    Result TryFlush();


    s32 Read(void* buffer, size_t size);
    s32 Write(const void* buffer, size_t size, bool flush);
    void Seek(s64 position, PositionBase base);
    s64 GetPosition() const;
    void SetPosition(s64 position);
    s64 GetSize() const;
    void SetSize(s64 size);
    void Flush();
    Result TryGetPosition(s64* pOut) const;
    Result TryInitialize(const wchar_t* pathName, bit32 mode);
    Result TryInitialize(const char* pathName, bit32 mode);
    void Initialize(const wchar_t* pathName, bit32 mode);
    void Initialize(const char* pathName, bit32 mode);
};

inline s32 FileBase::Read(void* buffer, size_t size){
    s32 ret;
    NN_ERR_THROW_FATAL_ALL(TryRead(&ret, buffer, size));
    return ret;
}

inline s32 FileBase::Write(const void* buffer, size_t size, bool flush){
    s32 ret;
    NN_ERR_THROW_FATAL_ALL(TryWrite(&ret, buffer, size, flush));
    return ret;
}

inline void FileBase::Seek(s64 position, PositionBase base){
    NN_ERR_THROW_FATAL_ALL(this->TrySeek(position, base));
}

inline s64 FileBase::GetPosition() const{
    s64 ret;
    NN_ERR_THROW_FATAL_ALL(TryGetPosition(&ret));
    return ret;
}

inline void FileBase::SetPosition(s64 position){
    NN_ERR_THROW_FATAL_ALL(TrySetPosition(position));
}

inline s64 FileBase::GetSize() const{
    s64 ret;
    NN_ERR_THROW_FATAL_ALL(TryGetSize(&ret));
    return ret;
}

inline void FileBase::SetSize(s64 size){
    NN_ERR_THROW_FATAL_ALL(TrySetSize(size));
}

inline void FileBase::Flush(){
    NN_ERR_THROW_FATAL_ALL(TryFlush());
}

inline Result FileBase::TryGetPosition(s64* pOut) const{
    *pOut = mPosition;
    return ResultSuccess();
}

inline Result FileBase::TryInitialize(const wchar_t* pathName, bit32 mode){
    this->mPosition = this->mSize = 0;
    return TryOpenImpl(pathName, mode);
}

inline Result FileBase::TryInitialize(const char* pathName, bit32 mode){
    static const size_t BUF_SIZE = MAX_FILE_PATH_LENGTH + 1;
    wchar_t buffer[BUF_SIZE];
    ConvertMbsToWcs(buffer, MAX_FILE_PATH_LENGTH, pathName);
    return TryInitialize(buffer, mode);
}

inline void FileBase::Initialize(const wchar_t* pathName, bit32 mode){
    Result result = TryInitialize(pathName, mode);
        if (result.IsFailure()){
        NN_SLOG_("file open failed: ");
        NN_SLOG_("%lls\n", pathName);
        NN_ERR_THROW_FATAL_ALL(result);
    }
}

inline void FileBase::Initialize(const char* pathName, bit32 mode){
    Result result = TryInitialize(pathName, mode);
        if (result.IsFailure()){
        NN_SLOG_("file open failed: ");
        NN_SLOG_("%s\n", pathName);
        NN_ERR_THROW_FATAL_ALL(result);
    }
}

}
}
}
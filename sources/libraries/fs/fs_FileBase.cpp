// Filename: fs_FileBase.cpp
//
// Project: Horizon

#include <nn/fs/fs_FileBase.h>
#include <nn/fs/fs_Parameters.h>
#include <nn/Result.h>

namespace nn{
namespace fs{
namespace detail{

Result FileBase::TryRead(s32* pOut, void* buffer, size_t size) {
    u32 n = 0;
    if (size != 0) {
        while (true) {
            s32 bytesRead;
            NN_UTIL_RETURN_IF_FAILED(FileBaseImpl::TryRead(&bytesRead, this->mPosition, buffer, size));

            n += bytesRead;
            mPosition += bytesRead;

            if (bytesRead == size || bytesRead == 0)
                break;

            buffer = (void*)((int)buffer + bytesRead);
            size -= bytesRead;
        }
    }
    *pOut = n;
    return ResultSuccess();
}

Result FileBase::TryWrite(int* pOut, const void* pBuffer, size_t size, bool flush=true){
    u32 n = 0;
    if (size != 0) {
        while (true) {
            s32 bytesRead;
            NN_UTIL_RETURN_IF_FAILED(FileBaseImpl::TryWrite(&bytesRead, this->mPosition, pBuffer, size, flush));

            n += bytesRead;
            mPosition += bytesRead;

            if (bytesRead == size || bytesRead == 0)
                break;

            pBuffer = (void*)((int)pBuffer + bytesRead);
            size -= bytesRead;
        }
    }
    *pOut = n;
    return ResultSuccess();
}

Result FileBase::TrySeek(s64 position, PositionBase base){
    switch (base){
        case BASE_BEGIN: break;
        case BASE_CURRENT: position += mPosition; break;
        case BASE_END:{
            s64 size;
            NN_UTIL_RETURN_IF_FAILED(TryGetSize(&size));
            position += size;
            break;
        }
        default: return ResultInvalidPositionBase();
    }
    return this->TrySetPosition(position);
}

Result FileBase::TrySetPosition(s64 position){
    if (position < 0){
        return ResultInvalidPosition();
    }
    if (position >= mSize){
        s64 size;
        NN_UTIL_RETURN_IF_FAILED(TryGetSize(&size));
        NN_TASSERT_(size == this->mSize);
        if (position > size){
            return ResultInvalidPosition();
        }
    }
    this->mPosition = position;
    return ResultSuccess();
}

Result FileBase::TryGetSize(s64* pOut) const{
    nn::util::Int64<s64>* pointer;
    s64 ret;
    Result res = this->FileBaseImpl::TryGetSize(&ret);
    if(res.IsSuccess()){
        this->mSize = ret;
        *pOut = ret;
    }
    else{
        this->mSize = 0LL;
    }
    return res;
}

Result FileBase::TrySetSize(s64 size){
    Result res = FileBaseImpl::TrySetSize(size);
    if (res.IsSuccess()){
        this->mSize = size;
        if (mSize < mPosition){
            this->mPosition = mSize;
        }
    }
    return res;
}

Result FileBase::TryFlush(){
    return this->FileBaseImpl::TryFlush();
}

}
}
}
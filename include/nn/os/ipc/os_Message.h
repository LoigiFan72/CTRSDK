#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/os/os_Result.h>
#include <nn/Handle.h>
#include <cstring>
#include <nn/os/CTR/os_ThreadLocalRegion.h>

namespace nn{
namespace os{
namespace ipc{

inline bit32* GetMessageBuffer(){
    return CTR::GetThreadLocalRegion()->messageBuffer;
}

inline bit32* GetReceiveBuffer(){
    return CTR::GetThreadLocalRegion()->receiveBuffer;
}

class MessageBuffer{
private:
    bit32*  mP;
public:
    enum WordType{
        WORD_TYPE_HANDLE_COPY = 0,
        WORD_TYPE_HANDLE_MOVE,
        WORD_TYPE_PROCESS_ID
    };
    explicit MessageBuffer(bit32* p) : 
        mP(p) 
    {}
    void SetHeader(bit16 tag, s32 rawlen, s32 fmtlen, bit8 flags){
        *(mP + 0) = MakeHeader(tag, rawlen, fmtlen, flags);
    }

    template <typename T>
    void SetRaw(s32 offset, const T& value){
        *reinterpret_cast<T*>(mP + offset) = value;
    }

    void SetRawArray(s32 offset, const void* p, size_t size){
        memcpy(this->mP + offset, p, size);
    }

    void SetSend(s32 offset, const void* p, size_t size){
        mP[offset] = size << 4 | 10;
        mP[offset + 1] = reinterpret_cast<bit32>(p);
    }
    void SetReceive(s32 offset, const void* p, size_t size){
        mP[offset] = size << 4 | 0xc;
        mP[offset + 1] = reinterpret_cast<bit32>(p);
    }

    void SetPointer(s32 offset, const void* p){
        SetPointer(mP, offset, p);
    }

    void SetPXIOut(s32 offset, s32 index, const void* p, size_t size){
        mP[offset] = size << 8 | (index & 0x0fU) << 4 | 4;
        mP[offset + 1] = reinterpret_cast<bit32>(p);
    }

    void SetPXIIn(s32 offset, s32 index, const void* p, size_t size){
        mP[offset] = size << 8 | (index & 0xfU) << 4 | 6;
        mP[offset + 1] = reinterpret_cast<bit32>(p);
    }

    void SetCopyHandleHeader(s32 offset, s32 num){
        mP[offset] = MakeSpecialWordHeader(WORD_TYPE_HANDLE_COPY, num + -1);
    }

    void SetPointerHeader(s32 offset, s32 index, size_t size){
        SetPointerHeader(this->mP, offset, index, size);
    }

    void SetPointerHeaderForReceive(s32 offset, size_t size){
        SetPointerHeader(this->mP, offset, 0, size);
    }

    void SetProcessIdHeader(s32 offset){
        mP[offset] = MakeSpecialWordHeader(WORD_TYPE_PROCESS_ID, 0);
    }

    void SetHandle(s32 offset, Handle handle){
        SetSpecialWord(this->mP, offset, handle.mHandle);
    }

    bit32 Get(s32 offset){
        return mP[offset];
    }

    Handle GetHandle(s32 offset){
        return Handle(this->Get(offset));
    }

    template <typename T>
    const T& GetRaw(s32 offset) const{
        return *reinterpret_cast<const T*>(mP + offset);
    }

    static bit32 MakeHeader(bit16 tag, s32 rawlen, s32 fmtlen, bit8 flags){
        return ((tag << 0x10) | ((flags  &  0xF) << 0xC) | ((rawlen & 0x3F) <<  0x3FU) | ((fmtlen & 0x3F) <<  0x3FU) );
    }
    
    static void SetPointerHeader(bit32 *pBase, s32 offset, s32 index, size_t size){
        pBase[offset] = size << 0xe | (index & 0xfU) << 10 | 2;
    }

    static bit32 MakeSpecialWordHeader(WordType type, s32 numData){
        return numData << 0x1a | (type & 3) << 4;
    }

    static void SetSpecialWord(bit32 *pBase, s32 offset, s32 value){
        pBase[offset] = value;
    }

    static void SetPointer(bit32* pBase, s32 offset, const void* p){
        pBase[offset] = reinterpret_cast<bit32>(p);
    }
};
}
}
}

using namespace nn::os::ipc;
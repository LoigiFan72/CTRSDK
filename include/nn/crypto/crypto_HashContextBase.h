#pragma once

#include <nn/types.h>

namespace nn{
namespace crypto{
class HashContextBase{
public:
    virtual void Initialize() {}
    virtual void Finalize() {}
    virtual void Update(const void* pData, size_t size) {}
    virtual size_t GetHashSize() { return 0; }
    virtual void GetHash(void* pOut) {}

protected:
    HashContextBase() {}
    virtual ~HashContextBase() {}
    virtual void ProcessBlock() {}
};

inline uint Convert32HToBE(uint he){
    return he << 0x18 | (he >> 8 & 0xff) << 0x10 | (he >> 0x10 & 0xff) << 8 | he >> 0x18;
}

inline static u32 RotateLeft32(int shift, u32 value){
    return (u32)((value << shift) | (value >> (u32)(32 - shift)));
}

}
}
#pragma once

#include <nn/crypto/crypto_HashContextBase.h>

namespace nn{
namespace crypto{
class ShaBlock512BitContext : public HashContextBase{
public:
    static const size_t BLOCK_SIZE = 64;
    
    virtual void Update(const void* pData, size_t size);
    virtual void InitializeWithContext(const void*, u64) {}
protected:
    u8 mBlock[BLOCK_SIZE];
    u32 mPool;
    u32 mBlocksLow;
    u32 mBlocksHigh;

    void AddPadding();
    virtual void ProcessBlock() {}

    ShaBlock512BitContext() {}
    virtual ~ShaBlock512BitContext() {}
};
}
}
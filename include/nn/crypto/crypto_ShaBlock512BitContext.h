#pragma once

#include <nn/crypto/crypto_HashContextBase.h>

namespace nn{
namespace crypto{
class ShaBlock512BitContext : public HashContextBase
{
public:
    static const size_t BLOCK_SIZE = 64;
    
    virtual void Update(const void* pData, size_t size);
    virtual void InitializeWithContext(const void*, u64) {}
protected:
    u8 m_Block[BLOCK_SIZE];
    u32 m_Pool;
    u32 m_BlocksLow;
    u32 m_BlocksHigh;

    void AddPadding();
    virtual void ProcessBlock() {}

    ShaBlock512BitContext() {}
    virtual ~ShaBlock512BitContext() {}
};
}
}
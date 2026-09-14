#pragma once

#include <nn/crypto/crypto_ShaBlock512BitContext.h>

namespace nn{
namespace crypto{
class Sha256Context : public ShaBlock512BitContext{
public:
    static const size_t HASH_SIZE = 32; // 256 bits

    Sha256Context() {}
    virtual ~Sha256Context() {}

    virtual void Initialize();
    virtual void Finalize() {}
    virtual void Update(const void* pData, size_t size);
    virtual void GetHash(void* pOut);
    virtual size_t GetHashSize() { return HASH_SIZE; }
    virtual void InitializeWithContext(const void* pContext, u64 size);

protected:
    virtual void ProcessBlock();
private:
    u32 m_H[8];
};

void CalculateSha256(void* pOut, const void* pData, size_t size);

}
}
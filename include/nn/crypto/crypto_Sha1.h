#pragma once

#include <nn/crypto/crypto_HashContextBase.h>
#include <nn/crypto/crypto_ShaBlock512BitContext.h>

namespace nn{ 
namespace crypto{


class Sha1Context : public ShaBlock512BitContext{
public:
    static const size_t HASH_SIZE  = (160/8); // 64 bits
    
private:
    u32     mH[5];
    
protected:
    virtual void ProcessBlock();
public:
    Sha1Context() {}
    virtual ~Sha1Context() {};
    virtual void Initialize();
    virtual void InitializeWithContext(const void *pContext, u64 size);
    virtual void Finalize() {}
    virtual void Update(const void* pData, size_t size);
    virtual size_t GetHashSize() { return HASH_SIZE; }
    virtual void GetHash(void* pOut);
    
};

void CalculateSha1(void *pOut, const void *pData, size_t size);

}
}
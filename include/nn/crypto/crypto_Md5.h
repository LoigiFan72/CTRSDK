#pragma once

#include <nn/crypto/crypto_HashContextBase.h>

namespace nn{ 
namespace crypto{

class Md5Context : public HashContextBase{
public:
    static const size_t HASH_SIZE  = (128/8);  // 128bit
    
private:
    union{
        struct{
            u32 a, b, c, d;
        } p;
        u32 state[4];
    } m_x;
    size_t m_size;
    union{
        u32 buffer32[16];
        u8  buffer8[64];
    } m_y;
    
protected:
    virtual void ProcessBlock();
    
public:
    Md5Context() {}
    virtual ~Md5Context() {};
    virtual void Initialize();
    virtual void Finalize() {}
    virtual void Update(const void* pData, size_t size);
    virtual size_t GetHashSize() { return HASH_SIZE; }
    virtual size_t GetHashLength() { return this->GetHashSize(); }
    virtual void GetHash(void* pOut);
};

void CalculateMd5(void *pOut, const void *pData, size_t size);

}
}
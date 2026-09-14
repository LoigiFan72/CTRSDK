// Filename: crypto_Sha1.cpp
//
// Project: Horizon

#include <nn/crypto/crypto_Sha1.h>
#include <nn/Assert.h>

namespace nn{
namespace crypto{

void Sha1Context::Initialize()
{
    m_BlocksLow  = 0;
    m_BlocksHigh = 0;
    m_Pool = 0;
    m_H[0] = 0x67452301;
    m_H[1] = 0xEFCDAB89;
    m_H[2] = 0x98BADCFE;
    m_H[3] = 0x10325476;
    m_H[4] = 0xC3D2E1F0;
}

void Sha1Context::InitializeWithContext(const void *pContext, u64 size)
{
    size /= BLOCK_SIZE;
    m_BlocksLow = size & 0xFFFFFFFF;
    m_BlocksHigh = size >> 32;
    m_Pool = 0;
    m_H[0] = Convert32HToBE(*((u32*)pContext + 0));
    m_H[1] = Convert32HToBE(*((u32*)pContext + 1));
    m_H[2] = Convert32HToBE(*((u32*)pContext + 2));
    m_H[3] = Convert32HToBE(*((u32*)pContext + 3));
    m_H[4] = Convert32HToBE(*((u32*)pContext + 4));
}

void Sha1Context::Update(const void* pData, size_t length)
{
    NN_TASSERT_(pData != 0);
    NN_MIN_TASSERT_(length, 1);

    this->ShaBlock512BitContext::Update(pData, length);
}

void Sha1Context::GetHash(void *pOut)
{
    NN_TASSERT_(pOut != 0);
    NN_ALIGN_TASSERT_(pOut, 4);

    this->AddPadding();

    u32* pDst = reinterpret_cast<u32*>(pOut);
    pDst[0] = Convert32HToBE(this->m_H[0]);
    pDst[1] = Convert32HToBE(this->m_H[1]);
    pDst[2] = Convert32HToBE(this->m_H[2]);
    pDst[3] = Convert32HToBE(this->m_H[3]);
    pDst[4] = Convert32HToBE(this->m_H[4]);
}

void CalculateSha1(void *pOut, const void *pData, size_t length)
{
    Sha1Context context;
    
    context.Initialize();
    context.Update(pData, length);
    context.GetHash(pOut);
    context.Finalize();
}

}
}
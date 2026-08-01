// Filename: crypto_Sha1.cpp
//
// Project: Horizon

#include <nn/crypto/crypto_Sha1.h>
#include <nn/Assert.h>

namespace nn{
namespace crypto{

void Sha1Context::Initialize(){
    mBlocksLow  = 0;
    mBlocksHigh = 0;
    mPool = 0;
    mH[0] = 0x67452301;
    mH[1] = 0xEFCDAB89;
    mH[2] = 0x98BADCFE;
    mH[3] = 0x10325476;
    mH[4] = 0xC3D2E1F0;
}

void Sha1Context::InitializeWithContext(const void *pContext, u64 size){
    size /= BLOCK_SIZE;
    mBlocksLow = size & 0xFFFFFFFF;
    mBlocksHigh = size >> 32;
    mPool = 0;
    mH[0] = Convert32HToBE(*((u32*)pContext + 0));
    mH[1] = Convert32HToBE(*((u32*)pContext + 1));
    mH[2] = Convert32HToBE(*((u32*)pContext + 2));
    mH[3] = Convert32HToBE(*((u32*)pContext + 3));
    mH[4] = Convert32HToBE(*((u32*)pContext + 4));
}

void Sha1Context::Update(const void* pData, size_t length){
    NN_TASSERT_(pData != 0);
    NN_MIN_TASSERT_(length, 1);

    this->ShaBlock512BitContext::Update(pData, length);
}

void Sha1Context::GetHash(void *pOut){
    NN_TASSERT_(pOut != 0);
    NN_ALIGN_TASSERT_(pOut, 4);

    this->AddPadding();

    u32* pDst = reinterpret_cast<u32*>(pOut);
    pDst[0] = Convert32HToBE(this->mH[0]);
    pDst[1] = Convert32HToBE(this->mH[1]);
    pDst[2] = Convert32HToBE(this->mH[2]);
    pDst[3] = Convert32HToBE(this->mH[3]);
    pDst[4] = Convert32HToBE(this->mH[4]);
}

void CalculateSha1(void *pOut, const void *pData, size_t length){
    Sha1Context context;
    
    context.Initialize();
    context.Update(pData, length);
    context.GetHash(pOut);
    context.Finalize();
}

}
}
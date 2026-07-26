// Filename: crypto_ShaBlock512BitContext.cpp
//
// Project: Horizon

#include <nn/crypto/crypto_ShaBlock512BitContext.h>
#include <nn/nstd/ARMv6/nstd_String.h>
#include <cstring>

extern "C" void nnnstdMemCpy(void* pOut, const void* scrp, size_t size);
namespace nn{
namespace crypto{
namespace{
    u8 sPadding[64] ={
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
}
inline void CopyBytes(const u8* pSrc, u8* pDst, size_t length){
    nstd::ARMv6::MemCpy(pDst, pSrc, length);
}

void ShaBlock512BitContext::Update(const void* pData, size_t length){
    const u8* pInput = reinterpret_cast<const u8*>(pData);
    
    while (length > 0){
        u32 rest = BLOCK_SIZE - mPool;
        u8 *dat;
        if (rest > length){
            rest = length;
        }
        dat = mBlock;
        CopyBytes(pInput, &dat[this->mPool], rest);
        pInput += rest;
        length -= rest;
        mPool  += rest;

        if (mPool >= BLOCK_SIZE){
            this->ProcessBlock();
            mPool = 0;
            ++mBlocksLow;
            if (!mBlocksLow){
                ++mBlocksHigh;
            }
        }
    }
}

void ShaBlock512BitContext::AddPadding(){
    u32 size;
    u32 pool;
    u32 footer[2];
    footer[1] = Convert32HToBE(this->mBlocksLow * 0x200 + this->mPool * 8);
    footer[0] = Convert32HToBE(this->mBlocksHigh * 0x200 + (this->mBlocksLow >> 0x17));
    pool = this->mPool;
    if(pool < 0x38)
        size = 0x38 - pool;
    else
        size = 0x78 - pool;
    this->Update(sPadding,size);
    this->Update(footer,8);
}
}
}
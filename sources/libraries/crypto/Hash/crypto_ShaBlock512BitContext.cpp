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
    u8 s_Padding[64] =
    {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
}
inline void CopyBytes(const u8* pSrc, u8* pDst, size_t length)
{
    nstd::ARMv6::MemCpy(pDst, pSrc, length);
}

void ShaBlock512BitContext::Update(const void* pData, size_t length)
{
    const u8* pInput = reinterpret_cast<const u8*>(pData);
    
    while (length > 0)
    {
        u32 rest = BLOCK_SIZE - m_Pool;
        u8 *dat;
        if (rest > length)
        {
            rest = length;
        }
        dat = m_Block;
        CopyBytes(pInput, &dat[this->m_Pool], rest);
        pInput += rest;
        length -= rest;
        m_Pool  += rest;

        if (m_Pool >= BLOCK_SIZE)
        {
            this->ProcessBlock();
            m_Pool = 0;
            ++m_BlocksLow;
            if (!m_BlocksLow)
            {
                ++m_BlocksHigh;
            }
        }
    }
}

void ShaBlock512BitContext::AddPadding()
{
    u32 size;
    u32 pool;
    u32 footer[2];
    footer[1] = Convert32HToBE(this->m_BlocksLow * 0x200 + this->m_Pool * 8);
    footer[0] = Convert32HToBE(this->m_BlocksHigh * 0x200 + (this->m_BlocksLow >> 0x17));
    pool = this->m_Pool;
    if(pool < 56)
        size = 56 - pool;
    else
        size = 120 - pool;
    this->Update(s_Padding,size);
    this->Update(footer,8);
}
}
}
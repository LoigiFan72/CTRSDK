// Filename: [ARM] crypto_Sha256ARM.cpp
//
// Project: Horizon

#include <nn/crypto/crypto_Sha256Context.h>
#include <nn/Assert.h>

namespace nn{
namespace crypto{
    u32 sSha256ConstantTable[64] ={
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

/*

"Hell no! Ima-I'm a white man I wrote my own fucking compiler im not a [CENSORED] like Linus, I-Wa-I'm a professional, I started at Ticket Master in 1990 and we wrote a compiler. The difference
in a professional and a amaetur [CE-] the diffi-difference between a amaetur and a professional is you write your own compiler, okay?" 

"I have a 20,000 line divine intellet op- uh.. compiler.. That operates just in time and ahead of time, you seem to be in denial why don't you fucking download my two meg- You can download
my two meg distribution that has all the source code on it and you can compile with my fucking complier..."


- King Terry the Terrible. 

How Nintendo Felt writing ProcessBlock:

*/

//! @note As seen comparing this in nico_nico and MLDT, this is ASM'd as unoptimized in nico_nico would produce different assembly.
//
//! @brief Processes a SHA-256 Block.
asm void Sha256Context::ProcessBlock(){
    push {r0,r4-r11,lr}
    sub sp,sp,#0x100
    add r4,r0,#0x4
    mov r2,#0x10
    add r3,sp,#0x0
loop_1
    ldr r5,[r4],#0x4
    subs r2,r2,#0x1
    rev r5,r5
    str r5,[r3],#0x4
    bne loop_1
    mov r2,#0x30
init_table
    subs r2,r2,#0x1
    ldr r10,[r3,#-0x3c]
    mov r11,r10, ror #0x7
    eor r11,r11,r10, ror #0x12
    ldr r4,[r3,#-0x40]
    eor r11,r11,r10, lsr #0x3
    ldr r5,[r3,#-0x8]
    mov r6,r5, ror #0x11
    add r10,r4,r11
    eor r6,r6,r5, ror #0x13
    ldr r8,[r3,#-0x1c]
    eor r5,r6,r5, lsr #0xa
    add r8,r8,r5
    add r8,r8,r10
    str r8,[r3],#0x4
    bne init_table
    add r0,r0,#0x50
    ldr r1,=__cpp(sSha256ConstantTable)
    ldmia r0,{r6,r7,r8,r9,r10,r11,r12,lr}
loop
    mov r5,r10, ror #0x6
    and r3,r10,r11
    bic r4,r12,r10
    eor r5,r5,r10, ror #0xb
    eor r3,r3,r4
    eor r5,r5,r10, ror #0x19
    add r5,r5,r3
    ldr r3,[r1,r2,lsl #0x2]
    add r3,r3,lr
    add r5,r5,r3
    ldr r3,[sp,r2,lsl #0x2]
    cpy lr,r12
    add r5,r5,r3
    cpy r12,r11
    eor r4,r7,r8
    cpy r11,r10
    add r10,r9,r5
    mov r3,r6, ror #0x2
    add r2,r2,#0x1
    eor r3,r3,r6, ror #0xd
    and r4,r4,r6
    and r0,r7,r8
    cpy r9,r8
    eor r3,r3,r6, ror #0x16
    eor r4,r4,r0
    cpy r8,r7
    add r3,r3,r4
    cpy r7,r6
    cmp r2,#0x40
    add r6,r5,r3
    blt loop
    ldr r0,[sp,#0x100]
    add r1,r0,#0x50
    ldmia r1,{r2,r3,r4,r5}
    add r2,r2,r6
    add r3,r3,r7
    add r4,r4,r8
    add r5,r5,r9
    stmia r1!,{r2,r3,r4,r5}
    ldmia r1,{r2,r3,r4,r5}
    add r2,r2,r10
    add r3,r3,r11
    add r4,r4,r12
    add r5,r5,lr
    stmia r1!,{r2,r3,r4,r5}
    add sp,sp,#0x104
    pop {r4-r11,pc}
}

void Sha256Context::Initialize(){
    mBlocksLow = 0;
    mBlocksHigh = 0;
    mPool = 0;
    mH[0] = 0x6a09e667;
    mH[1] = 0xbb67ae85;
    mH[2] = 0x3c6ef372;
    mH[3] = 0xa54ff53a;
    mH[4] = 0x510e527f;
    mH[5] = 0x9b05688c;
    mH[6] = 0x1f83d9ab;
    mH[7] = 0x5be0cd19;
}

void Sha256Context::Update(const void* pData, size_t size){
    NN_TASSERT_(pData != 0);
    NN_MIN_TASSERT_(size,0);

    ShaBlock512BitContext::Update(pData, size);
}

void Sha256Context::GetHash(void* pOut){
    NN_TASSERT_(pOut != 0);
    NN_ALIGN_TASSERT_(pOut, 4);

    this->AddPadding();
    u32* out = reinterpret_cast<u32*>(pOut);
    out[0] = Convert32HToBE(this->mH[0]);
    out[1] = Convert32HToBE(this->mH[1]);
    out[2] = Convert32HToBE(this->mH[2]);
    out[3] = Convert32HToBE(this->mH[3]);
    out[4] = Convert32HToBE(this->mH[4]);
    out[5] = Convert32HToBE(this->mH[5]);
    out[6] = Convert32HToBE(this->mH[6]);
    out[7] = Convert32HToBE(this->mH[7]);
}

void Sha256Context::InitializeWithContext(const void* pContext, u64 size){
    size /= BLOCK_SIZE;
    mBlocksLow = size & 0xFFFFFFFF;
    mBlocksHigh = size >> 32;
    mPool = 0;
    mH[0] = Convert32HToBE(*((u32*)pContext + 0));
    mH[1] = Convert32HToBE(*((u32*)pContext + 1));
    mH[2] = Convert32HToBE(*((u32*)pContext + 2));
    mH[3] = Convert32HToBE(*((u32*)pContext + 3));
    mH[4] = Convert32HToBE(*((u32*)pContext + 4));
    mH[5] = Convert32HToBE(*((u32*)pContext + 5));
    mH[6] = Convert32HToBE(*((u32*)pContext + 6));
    mH[7] = Convert32HToBE(*((u32*)pContext + 7));
}

void CalculateSha256(void* pOut, const void* pData, size_t size){
    Sha256Context context;
    context.Initialize();
    context.Update(pData, size);
    context.GetHash(pOut);
    context.Finalize();
}

}
}
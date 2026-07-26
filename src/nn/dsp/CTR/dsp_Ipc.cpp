// Filename: dsp_Ipc.cpp
//
// Project: Horizon

#include <nn/dsp/CTR/MPCore/dsp_Ipc.h>

namespace nn{
namespace dsp{
namespace CTR{

__asm Result DSP::ConvertProcessAddressFromDspDram(uptr _address,uptr *address){
    stmdb sp!,{r4,r5,r6,lr}
    cpy r5,r2
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r2,=0xC0040
    str r2,[r4,#0x80]!
    str r1,[r4,#0x4]
    ldr r0,[r0,#0x0]
    swi 0x32
    ands r1,r0,#0x80000000
    bmi conv_ret
    ldr r0,[r4,#0x8]
    str r0,[r5,#0x0]
    ldr r0,[r4,#0x4]
conv_ret
    ldmia sp!,{r4,r5,r6,pc}
}

__asm Result DSP::FlushDataCache(Handle clientProcess,uptr addr,size_t size){
    stmdb sp!,{r4,r5,r6,lr}
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r5,=0x130082
    mov r12,#0x0
    str r5,[r4,#0x80]!
    add r6,r4,#0x4
    str r1,[r4,#0x10]
    stmia r6,{r2,r3,r12}
    ldr r0,[r0,#0x0]
    swi 0x32
    and r1,r0,#0x80000000
    cmp r1,#0x0
    ldrge r0,[r4,#0x4]
    ldmia sp!,{r4,r5,r6,pc}
}

__asm Result DSP::GetSemaphoreEventHandle(Handle *handle){
    stmdb sp!,{r4,r5,r6,lr}
    cpy r5,r1
    mrc p15,0x0,r4,c13,c0,0x3
    mov r1,#0x160000
    str r1,[r4,#0x80]!
    ldr r0,[r0,#0x0]
    swi 0x32
    ands r1,r0,#0x80000000
    bmi semp_ret
    ldr r0,[r4,#0xc]
    str r0,[r5,#0x0]
    ldr r0,[r4,#0x4]
semp_ret
    ldmia sp!,{r4,r5,r6,pc}
}

__asm Result DSP::LoadComponent(const u8 *pComponent,size_t size,bit16 maskPram,bit16 maskDram,bool *pStatus){
    stmdb sp!,{r0,r1,r2,r3,r4,r5,r6,lr}
    ldr r5,[sp,#0x24]
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r3,=0x1100c2
    str r3,[r4,#0x80]!
    str r2,[r4,#0x4]
    ldrh r3,[sp,#0xc]
    strh r3,[r4,#0x8]
    ldrh r3,[sp,#0x20]
    strh r3,[r4,#0xc]
    mov r3,#0xa
    orr r2,r3,r2, lsl #0x4
    str r1,[r4,#0x14]
    str r2,[r4,#0x10]
    ldr r0,[r0,#0x0]
    swi 0x32
    ands r1,r0,#0x80000000
    bmi ret_comp
    ldrb r0,[r4,#0x8]
    strb r0,[r5,#0x0]
    ldr r0,[r4,#0x4]
ret_comp
    add sp,sp,#0x10
    ldmia sp!,{r4,r5,r6,pc}
}

__asm Result DSP::ReadPipeIfPossible(s32 port,s32 peer,u8 *pBuffer,u16 length,u16 *lengthRead){
    stmdb sp!,{r0,r1,r2,r4,r5,lr}
    cpy r5,r2
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r1,=0x10040
    str r1,[r4,#0x80]!
    ldrh r1,[sp,#0x4]
    strh r1,[r4,#0x4]
    ldr r0,[r0,#0x0]
    swi 0x32
    ands r1,r0,#0x80000000
    bmi pipe_ret
    ldrh r0,[r4,#0x8]
    strh r0,[r5,#0x0]
    ldr r0,[r4,#0x4]
pipe_ret
    add sp,sp,#0xc
    ldmia sp!,{r4,r5,pc}
}

__asm Result DSP::RecvData(u16 regNo, u16* pValue){
    stmdb sp!,{r0,r1,r2,r4,r5,lr}
    cpy r5,r2
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r1,=0x10040
    str r1,[r4,#0x80]!
    ldrh r1,[sp,#0x4]
    strh r1,[r4,#0x4]
    ldr r0,[r0,#0x0]
    swi 0x32
    ands r1,r0,#0x80000000
    bmi loc_38
    ldrh r0,[r4,#0x8]
    strh r0,[r5,#0x0]
    ldr r0,[r4,#0x4]
loc_38
    add sp,sp,#0xc
    ldmia sp!,{r4,r5,pc}
}

__asm Result DSP::RecvDataIsReady(u16 regNo,bool *pReady){
    stmdb sp!,{r4,r5,r6,lr}
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r5,=0x150082
    mov r12,#0x0
    str r5,[r4,#0x80]!
    add r6,r4,#0x4
    str r1,[r4,#0x10]
    stmia r6,{r2,r3,r12}
    ldr r0,[r0,#0x0]
    swi 0x32
    and r1,r0,#0x80000000
    cmp r1,#0x0
    ldrge r0,[r4,#0x4]
    ldmia sp!,{r4,r5,r6,pc}
}

__asm Result DSP::SetSemaphore(u16 mask){
    stmdb sp!,{r0,r1,r4,lr}
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r1,=0x70040
    str r1,[r4,#0x80]!
    ldrh r1,[sp,#0x4]
    strh r1,[r4,#0x4]
    ldr r0,[r0,#0x0]
    swi 0x32
    and r1,r0,#0x80000000
    cmp r1,#0x0
    ldrge r0,[r4,#0x4]
    add sp,sp,#0x8
    ldmia sp!,{r4,pc}
}

__asm Result DSP::SetSemaphoreMask(u16 mask){
    stmdb sp!,{r0,r1,r4,lr}
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r1,=0x170040
    str r1,[r4,#0x80]!
    ldrh r1,[sp,#0x4]
    strh r1,[r4,#0x4]
    ldr r0,[r0,#0x0]
    swi 0x32
    and r1,r0,#0x80000000
    cmp r1,#0x0
    ldrge r0,[r4,#0x4]
    add sp,sp,#0x8
    ldmia sp!,{r4,pc}
}

__asm Result DSP::RegisterInterruptEvents(Handle h, int, int){
    stmdb sp!,{r4,r5,r6,lr}
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r5,=0x150082
    mov r12,#0x0
    str r5,[r4,#0x80]!
    add r6,r4,#0x4
    str r1,[r4,#0x10]
    stmia r6,{r2,r3,r12}
    ldr r0,[r0,#0x0]
    swi 0x32
    and r1,r0,#0x80000000
    cmp r1,#0x0
    ldrge r0,[r4,#0x4]
    ldmia sp!,{r4,r5,r6,pc}
}

__asm Result DSP::UnloadComponent(){
    stmdb sp!,{r4,lr}
    mrc p15,0x0,r4,c13,c0,0x3
    mov r1,#0x120000
    str r1,[r4,#0x80]!
    ldr r0,[r0,#0x0]
    swi 0x32
    and r1,r0,#0x80000000
    cmp r1,#0x0
    ldrge r0,[r4,#0x4]
    ldmia sp!,{r4,pc}
}

__asm Result DSP::WriteProcessPipe(s32 port,u8 *pBuffer,size_t length){
    stmdb sp!,{r4,lr}
    mrc p15,0x0,r4,c13,c0,0x3
    ldr r12,=0xd0082
    str r12,[r4,#0x80]!
    add lr,r4,#0x4
    stmia lr,{r1,r3}
    mov r1,#0x400
    orr r1,r1,r3, lsl #0xe
    add lr,r4,#0xc
    orr r1,r1,#0x2
    stmia lr,{r1,r2}
    ldr r0,[r0,#0x0]
    swi 0x32
    and r1,r0,#0x80000000
    cmp r1,#0x0
    ldrge r0,[r4,#0x4]
    ldmia sp!,{r4,pc}
}

}
}
}
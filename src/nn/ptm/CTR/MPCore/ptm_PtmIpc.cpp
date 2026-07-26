// Filename: ptm_PtmIpc.cpp
//
// Project: Horizon

#include <nn/ptm/CTR/detail/ptm_PtmIpc.h>

namespace nn{
namespace ptm{
namespace CTR{
namespace detail{
namespace PtmIpc{
Handle sSession;

__asm Result GetStepHistory(ushort* pStepCounts, s32 numHours, nn::fnd::DateTime start){
    PUSH            {R4,LR}
    MRC             p15, 0, R4,c13,c0, 3
    LDR             R12, =0xB00C2
    STR             R12, [R4,#0x80]!
    ADD             LR, R4, #4
    STM             LR, {R1-R3}
    MOV             R2, #0xC
    STR             R0, [R4,#0x14]
    LDR             R0, =__cpp(&sSession)
    ORR             R1, R2, R1,LSL#5
    STR             R1, [R4,#0x10]
    LDR             R0, [R0]
    SVC             0x32 
    AND             R1, R0, #0x80000000
    CMP             R1, #0
    LDRGE           R0, [R4,#4]
    POP             {R4,PC}
}

}
}
}
}
}
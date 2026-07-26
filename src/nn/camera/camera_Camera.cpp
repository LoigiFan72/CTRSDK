// Filename: camera_Camera.cpp
//
// Project: Horizon

#include <nn/camera/CTR/camera_Camera.h>

namespace nn{
namespace camera{
namespace CTR{
namespace detail{

Handle Camera::sSession;

__asm Result Camera::Activate(u8){
    PUSH            {R0,R4,R5,LR}
    MRC             p15, 0, R4,c13,c0, 3
    LDR             R0, =0x003D0040
    STR             R0, [R4,#0x80]!
    LDRB            R0, [SP,#0x0]
    STRB            R0, [R4,#4]
    LDR             R0, =__cpp(&Camera::sSession)
    LDR             R0, [R0]
    SVC             0x32 ; '2'
    AND             R1, R0, #0x80000000
    CMP             R1, #0
    LDRGE           R0, [R4,#4]
    POP             {R3-R5,PC}
}

__asm Result Camera::GetActivatedCamera(u8* camera){
    PUSH            {R4-R6,LR}
    MOV             R5, R0
    MRC             p15, 0, R4,c13,c0, 3
    MOV             R0, #0x3B0000
    STR             R0, [R4,#0x80]!
    LDR             R0, =__cpp(&Camera::sSession)
    LDR             R0, [R0]
    SVC             0x32 ; '2'
    ANDS            R1, R0, #0x80000000
    BMI             locret_12A96C
    LDRB            R0, [R4,#8]
    STRB            R0, [R5]
    LDR             R0, [R4,#4]

locret_12A96C
    POP             {R4-R6,PC}
}

__asm Result Camera::GetSleepCamera(u8* camera){
    PUSH            {R4-R6,LR}
    MOV             R5, R0
    MRC             p15, 0, R4,c13,c0, 3
    MOV             R0, #0x3C0000
    STR             R0, [R4,#0x80]!
    LDR             R0, =__cpp(&Camera::sSession)
    LDR             R0, [R0]
    SVC             0x32 ; '2'
    ANDS            R1, R0, #0x80000000
    BMI             locret_12A8F4
    LDRB            R0, [R4,#8]
    STRB            R0, [R5]
    LDR             R0, [R4,#4]

locret_12A8F4
    POP             {R4-R6,PC}
}

__asm Result Camera::SetSleepCamera(u8){
    PUSH            {R0,R4,R5,LR}
    MRC             p15, 0, R4,c13,c0, 3
    LDR             R0, =0x130040
    STR             R0, [R4,#0x80]!
    LDRB            R0, [SP,#0x0]
    STRB            R0, [R4,#4]
    LDR             R0, =__cpp(&Camera::sSession)
    LDR             R0, [R0]
    SVC             0x32
    AND             R1, R0, #0x80000000
    CMP             R1, #0
    LDRGE           R0, [R4,#4]
    POP             {R3-R5,PC}
}

}
}
}
}
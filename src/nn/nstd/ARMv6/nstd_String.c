// Filename: [ARMv6] nstd_String.c
//
// Project: Horizon

#include <nn/nstd/ARMv6/nstd_String.h>

asm void nnnstdMemMove (void* pOut, const void* pIn, size_t size){
        SUBS    R3, R0, R1;
        CMPCS   R2, R3;
        BLS     nnnstdMemCpy;
        ADD     R0, R0, R2;
        ADD     R1, R1, R2;
        CMP     R2, #8;
        BHI     loc_2E8724;
        BEQ     loc_2E8710;
        CMP     R2, #4;
        BHI     loc_2E86A8;
        BEQ     loc_2E86DC;
        CMP     R2, #2;
        BCC     loc_2E8694;
        BEQ     loc_2E86D0;
        LDRH    R2, [ R1, # - 2 ] !;
        LDRB    R3, [ R1, # - 1 ] !;
        STRH    R2, [ R0, # - 2 ] !;
        STRB    R3, [ R0, # - 1 ] !;
        BX      LR;
loc_2E8694 //nf
        CMP    R2, #1; //nf
        BCC     locret_2E86A4;
        LDRB    R3, [ R1, # - 1 ] !;
        STRB    R3, [ R0, # - 1 ] !;
locret_2E86A4 //nf
                 BX      LR; //nf
loc_2E86A8 //nf
        CMP    R2, #6; //nf
        BEQ     loc_2E86FC;
        BCC     loc_2E86E8;
        LDR     R2, [ R1, # - 4 ] !;
        LDRH    R3, [ R1, # - 2 ] !;
        STR     R2, [ R0, # - 4 ] !;
        LDRB    R12, [ R1, # - 1 ] !;
        STRH    R3, [ R0, # - 2 ] !;
        STRB    R12, [ R0, # - 1 ] !;
        BX      LR;
loc_2E86D0 //nf
        LDRH   R3, [ R1, # - 2 ] !; //nf
        STRH    R3, [ R0, # - 2 ] !;
        BX      LR;
loc_2E86DC //nf
        LDR    R3, [ R1, # - 4 ] !; //nf
        STR     R3, [ R0, # - 4 ] !;
        BX      LR;
loc_2E86E8 //nf
        LDR    R2, [ R1, # - 4 ] !; //nf
        LDRB    R3, [ R1, # - 1 ] !;
        STR     R2, [ R0, # - 4 ] !;
        STRB    R3, [ R0, # - 1 ] !;
        BX      LR;
loc_2E86FC //nf
        LDR    R2, [ R1, # - 4 ] !; //nf
        LDRH    R3, [ R1, # - 2 ] !;
        STR     R2, [ R0, # - 4 ] !;
        STRH    R3, [ R0, # - 2 ] !;
        BX      LR;
loc_2E8710 //nf
        LDR    R2, [ R1, # - 4 ] !; //nf
        LDR     R3, [ R1, # - 4 ] !;
        STR     R2, [ R0, # - 4 ] !;
        STR     R3, [ R0, # - 4 ] !;
        BX      LR;
loc_2E8724 //nf
        TST    R1, #2; //nf
        SUBNE   R2, R2, #2;
        LDRBNE  R3, [ R1, # - 1 ] !;
        LDRBNE  R12, [ R1, # - 1 ] !;
        STRBNE  R3, [ R0, # - 1 ] !;
        STRBNE  R12, [ R0, # - 1 ] !;
        TST     R1, #1;
        SUBNE   R2, R2, #1;
        LDRBNE  R3, [ R1, # - 1 ] !;
        STRBNE  R3, [ R0, # - 1 ] !;
        ANDS    R3, R0, #3;
        BNE     loc_2E87BC;
        PUSH { R4 - R10, LR };//nf
        SUBS    R2, R2, #0x20;
        BCC     loc_2E877C;
loc_2E8760 //nf
        LDMDB R1 !, { R4 - R7 }; //nf
        LDMDB   R1 !, { R8 - R10, LR };
        PLD [R1, # - 0x40]; //nf
        SUBS    R2, R2, #0x20;
        STMDB   R0 !, { R4 - R7 };
        STMDB   R0 !, { R8 - R10, LR };
        BCS     loc_2E8760;
loc_2E877C //nf
        MOVS   R12, R2, LSL #28; //nf
        LDMDBCS R1 !, { R3, R4, R12, LR };
        STMDBCS R0 !, { R3, R4, R12, LR };
        LDMDBMI R1 !, { R3, R4 };
        STMDBMI R0 !, { R3, R4 };
        POP { R4 - R10, LR };//nf
        MOVS    R12, R2, LSL #30;
        LDRCS   R3, [ R1, # - 4 ] !;
        STRCS   R3, [ R0, # - 4 ] !;
        BXEQ    LR;
        MOVS    R2, R2, LSL #31;
        LDRHCS  R3, [ R1, # - 2 ] !;
        LDRBMI  R2, [ R1, # - 1 ];
        STRHCS  R3, [ R0, # - 2 ] !;
        STRBMI  R2, [ R0, # - 1 ];
        BX      LR;
loc_2E87BC //nf
        BIC    R0, R0, #3; //nf
        CMP     R3, #2;
        BEQ     loc_2E8868;
        BHI     loc_2E8904;
        LDR     R12, [R0];
        MOV     R12, R12, LSR #8;
        MOV     R12, R12, LSL #8;
        PUSH { R4 - R10, LR };//nf
        SUBS    R2, R2, #0x20;
loc_2E87E0 //nf
        LDMDBGE R1!, {R4-R10,LR}; //nf
        MOVGE   R3, LR, LSL #8;
        ORRGE   LR, R12, LR, LSR #24;
        MOVGE   R12, R10, LSL #8;
        ORRGE   R10, R3, R10, LSR #24;
        MOVGE   R3, R9, LSL #8;
        ORRGE   R9, R12, R9, LSR #24;
        MOVGE   R12, R8, LSL #8;
        ORRGE   R8, R3, R8, LSR #24;
        MOVGE   R3, R7, LSL #8;
        ORRGE   R7, R12, R7, LSR #24;
        MOVGE   R12, R6, LSL #8;
        ORRGE   R6, R3, R6, LSR #24;
        MOVGE   R3, R5, LSL #8;
        ORRGE   R5, R12, R5, LSR #24;
        MOVGE   R12, R4, LSL #8;
        ORRGE   R4, R3, R4, LSR #24;
        STMDAGE R0 !, { R4 - R10, LR };
        SUBSGE  R2, R2, #0x20;
        BGE     loc_2E87E0;
        ADD     R2, R2, #0x20;
        POP { R4 - R10, LR };//nf
        SUBS    R2, R2, #4;
loc_2E883C //nf
        LDRGE   R3, [R1,#-4]!; //nf
        ORRGE   R12, R12, R3, LSR #24;
        STRGE   R12, [R0], # - 4;
        MOVGE   R12, R3, LSL #8;
        SUBSGE  R2, R2, #4;
        BGE     loc_2E883C;
        ADD     R2, R2, #4;
        ADD     R0, R0, #4;
        ADD     R1, R1, #3;
        ADD     R2, R2, #3;
        B       loc_2E899C;
loc_2E8868 //nf
        LDR     R12, [R0]; //nf
        MOV     R12, R12, LSR #16;
        MOV     R12, R12, LSL #16;
        PUSH { R4 - R10, LR };//nf
        SUBS    R2, R2, #0x20;
loc_2E887C //nf
        LDMDBGE R1!, {R4-R10,LR}; //nf
        MOVGE   R3, LR, LSL #16;
        ORRGE   LR, R12, LR, LSR #16;
        MOVGE   R12, R10, LSL #16;
        ORRGE   R10, R3, R10, LSR #16;
        MOVGE   R3, R9, LSL #16;
        ORRGE   R9, R12, R9, LSR #16;
        MOVGE   R12, R8, LSL #16;
        ORRGE   R8, R3, R8, LSR #16;
        MOVGE   R3, R7, LSL #16;
        ORRGE   R7, R12, R7, LSR #16;
        MOVGE   R12, R6, LSL #16;
        ORRGE   R6, R3, R6, LSR #16;
        MOVGE   R3, R5, LSL #16;
        ORRGE   R5, R12, R5, LSR #16;
        MOVGE   R12, R4, LSL #16;
        ORRGE   R4, R3, R4, LSR #16;
        STMDAGE R0 !, { R4 - R10, LR };
        SUBSGE  R2, R2, #0x20;
        BGE     loc_2E887C;
        ADD     R2, R2, #0x20;
        POP { R4 - R10, LR };//nf
        SUBS    R2, R2, #4;
loc_2E88D8 //nf
        LDRGE   R3, [R1,#-4]!; //nf
        ORRGE   R12, R12, R3, LSR #16;
        STRGE   R12, [R0], # - 4;
        MOVGE   R12, R3, LSL #16;
        SUBSGE  R2, R2, #4;
        BGE     loc_2E88D8;
        ADD     R2, R2, #4;
        ADD     R0, R0, #4;
        ADD     R1, R1, #2;
        ADD     R2, R2, #2;
        B       loc_2E899C;
loc_2E8904 //nf
        LDR    R12, [R0]; //nf
        MOV     R12, R12, LSR #24;
        MOV     R12, R12, LSL #24;
        PUSH { R4 - R10, LR };//nf
        SUBS    R2, R2, #0x20;
loc_2E8918 //nf
        LDMDBGE R1!, {R4-R10,LR}; //nf
        MOVGE   R3, LR, LSL #24;
        ORRGE   LR, R12, LR, LSR #8;
        MOVGE   R12, R10, LSL #24;
        ORRGE   R10, R3, R10, LSR #8;
        MOVGE   R3, R9, LSL #24;
        ORRGE   R9, R12, R9, LSR #8;
        MOVGE   R12, R8, LSL #24;
        ORRGE   R8, R3, R8, LSR #8;
        MOVGE   R3, R7, LSL #24;
        ORRGE   R7, R12, R7, LSR #8;
        MOVGE   R12, R6, LSL #24;
        ORRGE   R6, R3, R6, LSR #8;
        MOVGE   R3, R5, LSL #24;
        ORRGE   R5, R12, R5, LSR #8;
        MOVGE   R12, R4, LSL #24;
        ORRGE   R4, R3, R4, LSR #8;
        STMDAGE R0 !, { R4 - R10, LR };
        SUBSGE  R2, R2, #0x20;
        BGE     loc_2E8918;
        ADD     R2, R2, #0x20;
        POP { R4 - R10, LR };//nf
        SUBS    R2, R2, #4;
loc_2E8974 //nf
        LDRGE  R3, [R1,#-4]!; //nf
        ORRGE   R12, R12, R3, LSR #8;
        STRGE   R12, [R0], # - 4;
        MOVGE   R12, R3, LSL #24;
        SUBSGE  R2, R2, #4;
        BGE     loc_2E8974;
        ADD     R2, R2, #4;
        ADD     R0, R0, #4;
        ADD     R1, R1, #1;
        ADD     R2, R2, #1;
loc_2E899C //nf
        TST    R2, #4; //nf
        LDRBNE  R3, [ R1, # - 1 ] !;
        LDRBNE  R12, [ R1, # - 1 ] !;
        STRBNE  R3, [ R0, # - 1 ] !;
        STRBNE  R12, [ R0, # - 1 ] !;
        LDRBNE  R3, [ R1, # - 1 ] !;
        LDRBNE  R12, [ R1, # - 1 ] !;
        STRBNE  R3, [ R0, # - 1 ] !;
        STRBNE  R12, [ R0, # - 1 ] !;
        TST     R2, #2;
        LDRBNE  R3, [ R1, # - 1 ] !;
        LDRBNE  R12, [ R1, # - 1 ] !;
        STRBNE  R3, [ R0, # - 1 ] !;
        STRBNE  R12, [ R0, # - 1 ] !;
        TST     R2, #1;
        LDRBNE  R3, [ R1, # - 1 ] !;
        STRBNE  R3, [ R0, # - 1 ] !;
        BX      LR;
}

// Copys Memory.
asm void nnnstdMemCpy (void* pOut, const void* pIn, size_t size){
        CMP    R2, #8;
        BHI    loc_29241C;
        BEQ    loc_292408;
        CMP    R2, #4;
        BHI    loc_2923A0;
        BEQ    loc_2923D4;
        CMP    R2, #2;
        BCC    loc_29238C;
        BEQ    loc_2923C8;
        LDRH   R2, [R1], #2;
        LDRB   R3, [R1], #1;
        STRH   R2, [R0], #2;
        STRB   R3, [R0], #1;
        BX     LR;


loc_29238C // nf             
        CMP    R2, #1; // nf
        BCC    locret_29239C;
        LDRB   R3, [R1], #1;
        STRB   R3, [R0], #1;

locret_29239C // nf        
        BX LR; // nf 


loc_2923A0 // nf          
        CMP    R2, #6; // nf
        BEQ    loc_2923F4;
        BCC    loc_2923E0;
        LDR    R2, [R1], #4;
        LDRH   R3, [R1], #2;
        STR    R2, [R0], #4;
        LDRB   R12, [R1], #1;
        STRH   R3, [R0], #2;
        STRB   R12, [R0], #1;
        BX     LR;


loc_2923C8 // nf 
        LDRH R3, [R1], #2; // nf
        STRH   R3, [R0], #2;
        BX     LR;


loc_2923D4 // nf 
        LDR    R3, [R1], #4; // nf
        STR    R3, [R0], #4;
        BX     LR;


loc_2923E0 // nf 
        LDR    R2, [R1], #4; // nf
        LDRB   R3, [R1], #1;
        STR    R2, [R0], #4;
        STRB   R3, [R0], #1;
        BX     LR;


loc_2923F4 // nf 
        LDR    R2, [R1], #4; // nf
        LDRH   R3, [R1], #2;
        STR    R2, [R0], #4;
        STRH   R3, [R0], #2;
        BX     LR;


loc_292408 // nf 
        LDR    R2, [R1], #4; // nf
        LDR    R3, [R1], #4;
        STR    R2, [R0], #4;
        STR    R3, [R0], #4;
        BX     LR;


loc_29241C // nf  
        TST    R1, #1; // nf
        SUBNE  R2, R2, #1;
        LDRBNE R3, [R1], #1;
        STRBNE R3, [R0], #1;
        TST    R1, #2;
        SUBNE  R2, R2, #2;
        LDRBNE R3, [R1], #1;
        LDRBNE R12, [R1], #1;
        STRBNE R3, [R0], #1;
        STRBNE R12, [R0], #1;
        ANDS   R3, R0, #3;
        BNE    loc_2924B4;
        PUSH { R4 - R10, LR };//nf
        SUBS   R2, R2, #0x20;
        BCC    loc_292474;

loc_292458 // nf 
        LDM  R1!, {R4-R7}; // nf
        LDM    R1 !, { R8 - R10, LR };
        PLD [R1, #0x40]; //nf
        SUBS   R2, R2, #0x20;
        STM    R0 !, { R4 - R7 };
        STM    R0 !, { R8 - R10, LR };
        BCS    loc_292458;

loc_292474 // nf 
        MOVS  R12, R2, LSL #28; // nf
        LDMCS  R1 !, { R3, R4, R12, LR };
        STMCS  R0 !, { R3, R4, R12, LR };
        LDMMI  R1 !, { R3, R4 };
        STMMI  R0 !, { R3, R4 };
        POP { R4 - R10, LR };//nf
        MOVS   R12, R2, LSL #30;
        LDRCS  R3, [R1], #4;
        STRCS  R3, [R0], #4;
        BXEQ   LR;
        MOVS   R2, R2, LSL #31;
        LDRHCS R3, [R1], #2;
        LDRBMI R2, [R1], #1;
        STRHCS R3, [R0], #2;
        STRBMI R2, [R0], #1;
        BX     LR;


loc_2924B4 // nf 
        BIC    R0, R0, #3; // nf
        CMP    R3, #2;
        BEQ    loc_292560;
        BHI    loc_2925FC;
        LDR    R12, [R0];
        MOV    R12, R12, LSL #24;
        MOV    R12, R12, LSR #24;
        SUBS   R2, R2, #0x20;
        BLT    loc_292530;
        PUSH { R4 - R10, LR };//nf

loc_2924DC // nf 
        LDMGE  R1!, {R4-R10,LR}; // nf
        MOVGE  R3, R4, LSR #24;
        ORRGE  R4, R12, R4, LSL #8;
        MOVGE  R12, R5, LSR #24;
        ORRGE  R5, R3, R5, LSL #8;
        MOVGE  R3, R6, LSR #24;
        ORRGE  R6, R12, R6, LSL #8;
        MOVGE  R12, R7, LSR #24;
        ORRGE  R7, R3, R7, LSL #8;
        MOVGE  R3, R8, LSR #24;
        ORRGE  R8, R12, R8, LSL #8;
        MOVGE  R12, R9, LSR #24;
        ORRGE  R9, R3, R9, LSL #8;
        MOVGE  R3, R10, LSR #24;
        ORRGE  R10, R12, R10, LSL #8;
        MOVGE  R12, LR, LSR #24;
        ORRGE  LR, R3, LR, LSL #8;
        STMGE  R0 !, { R4 - R10, LR };
        SUBSGE R2, R2, #0x20;
        BGE    loc_2924DC;
        POP { R4 - R10, LR };//nf

loc_292530                         // nf 
                ADD     R2, R2, #0x20 ; // nf
        SUBS   R2, R2, #4;

loc_292538 // nf 
        LDRGE   R3, [R1],#4; // nf
        ORRGE  R12, R12, R3, LSL #8;
        STRGE  R12, [R0], #4;
        MOVGE  R12, R3, LSR #24;
        SUBSGE R2, R2, #4;
        BGE    loc_292538;
        ADD    R2, R2, #4;
        SUB    R1, R1, #1;
        ADD    R2, R2, #1;
        B      loc_292694;


loc_292560 // nf 
        LDR    R12, [R0]; // nf
        MOV    R12, R12, LSL #16;
        MOV    R12, R12, LSR #16;
        SUBS   R2, R2, #0x20;
        BLT    loc_2925CC;
        PUSH { R4 - R10, LR };//nf

loc_292578 // nf 
        LDMGE  R1!, {R4-R10,LR}; // nf
        MOVGE  R3, R4, LSR #16;
        ORRGE  R4, R12, R4, LSL #16;
        MOVGE  R12, R5, LSR #16;
        ORRGE  R5, R3, R5, LSL #16;
        MOVGE  R3, R6, LSR #16;
        ORRGE  R6, R12, R6, LSL #16;
        MOVGE  R12, R7, LSR #16;
        ORRGE  R7, R3, R7, LSL #16;
        MOVGE  R3, R8, LSR #16;
        ORRGE  R8, R12, R8, LSL #16;
        MOVGE  R12, R9, LSR #16;
        ORRGE  R9, R3, R9, LSL #16;
        MOVGE  R3, R10, LSR #16;
        ORRGE  R10, R12, R10, LSL #16;
        MOVGE  R12, LR, LSR #16;
        ORRGE  LR, R3, LR, LSL #16;
        STMGE  R0 !, { R4 - R10, LR };
        SUBSGE R2, R2, #0x20;
        BGE    loc_292578;
        POP { R4 - R10, LR };//nf

loc_2925CC // nf 
        ADD    R2, R2, #0x20; // nf
        SUBS   R2, R2, #4;

loc_2925D4 // nf 
        LDRGE  R3, [R1], #4; // nf
        ORRGE  R12, R12, R3, LSL #16;
        STRGE  R12, [R0], #4;
        MOVGE  R12, R3, LSR #16;
        SUBSGE R2, R2, #4;
        BGE    loc_2925D4;
        ADD    R2, R2, #4;
        SUB    R1, R1, #2;
        ADD    R2, R2, #2;
        B      loc_292694;


loc_2925FC // nf 
        LDR    R12, [R0]; // nf
        MOV    R12, R12, LSL #8;
        MOV    R12, R12, LSR #8;
        SUBS   R2, R2, #0x20;
        BLT    loc_292668;
        PUSH { R4 - R10, LR };//nf

loc_292614 // nf 
        LDMGE  R1!, {R4-R10,LR}; // nf
        MOVGE  R3, R4, LSR #8;
        ORRGE  R4, R12, R4, LSL #24;
        MOVGE  R12, R5, LSR #8;
        ORRGE  R5, R3, R5, LSL #24;
        MOVGE  R3, R6, LSR #8;
        ORRGE  R6, R12, R6, LSL #24;
        MOVGE  R12, R7, LSR #8;
        ORRGE  R7, R3, R7, LSL #24;
        MOVGE  R3, R8, LSR #8;
        ORRGE  R8, R12, R8, LSL #24;
        MOVGE  R12, R9, LSR #8;
        ORRGE  R9, R3, R9, LSL #24;
        MOVGE  R3, R10, LSR #8;
        ORRGE  R10, R12, R10, LSL #24;
        MOVGE  R12, LR, LSR #8;
        ORRGE  LR, R3, LR, LSL #24;
        STMGE  R0 !, { R4 - R10, LR };
        SUBSGE R2, R2, #0x20;
        BGE    loc_292614;
        POP { R4 - R10, LR };//nf

loc_292668 // nf 
        ADD    R2, R2, #0x20; // nf
        SUBS   R2, R2, #4;

loc_292670 // nf 
        LDRGE  R3, [R1],#4; // nf
        ORRGE  R12, R12, R3, LSL #24;
        STRGE  R12, [R0], #4;
        MOVGE  R12, R3, LSR #8;
        SUBSGE R2, R2, #4;
        BGE    loc_292670;
        ADD    R2, R2, #4;
        SUB    R1, R1, #3;
        ADD    R2, R2, #3;

loc_292694 // nf 
        TST    R2, #4; // nf
        LDRBNE R3, [R1], #1;
        LDRBNE R12, [R1], #1;
        STRBNE R3, [R0], #1;
        STRBNE R12, [R0], #1;
        LDRBNE R3, [R1], #1;
        LDRBNE R12, [R1], #1;
        STRBNE R3, [R0], #1;
        STRBNE R12, [R0], #1;
        TST    R2, #2;
        LDRBNE R3, [R1], #1;
        LDRBNE R12, [R1], #1;
        STRBNE R3, [R0], #1;
        STRBNE R12, [R0], #1;
        TST    R2, #1;
        LDRBNE R3, [R1], #1;
        STRBNE R3, [R0], #1;
        BX     LR;
}
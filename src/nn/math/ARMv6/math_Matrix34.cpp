// Filename: math_Matrix.cpp
//
// Project: Horizon

#include <nn/math/math_Matrix34.h>

namespace nn{ 
namespace math{ 
namespace ARMv6{
asm MTX34* MTX34CopyAsm(MTX34* , const MTX34*){
    CMP         r1,r0 // Are p and pOut the same, or no?
    BXEQ        lr // If so, return.
    VLDMIA      r1!,{s0-s5} // Load all to shorten stall times
    MOV         r2, r0
    VLDMIA      r1,{s6-s11}    
    VSTMIA      r2!,{s0-s5}     
    VSTMIA      r2,{s6-s11}    
    BX          lr
}

asm MTX34* MTX34MultAsm(MTX34*, const MTX34*, const MTX34*){
    VPUSH       {d8-d10} // Save regs

    VLDR.F32    s3,[r1,#4*4*0+4*3]  // Matrix p1[0][3]
    VLDR.F32    s7,[r1,#4*4*1+4*3]  // Matrix p1[1][3]
    VLDR.F32    s11,[r1,#4*4*2+4*3] // Matrix p1[2][3]

    VLDMIA      r2!,{s12-s15} // Save Matrix p2 into [S12-S15] regs

    VLDR.F32    s20,[r1,#4*4*0+4*0] // Matrix p1[0][0]
    VLDR.F32    s21,[r1,#4*4*1+4*0] // Matrix p1[1][0]
    VMUL.F32    s0,s12,s20
    VMUL.F32    s1,s13,s20
    VMUL.F32    s2,s14,s20
    VMLA.F32    s3,s15,s20

    VLDR.F32    s20,[r1,#4*4*2+4*0] // Matrix p1[2][0]
    VMUL.F32    s4,s12,s21
    VMUL.F32    s5,s13,s21
    VMUL.F32    s6,s14,s21
    VMLA.F32    s7,s15,s21
    VLDMIA      r2!,{s16-s19}       // Save Matrix p2 into [S16-S19] regs

    VLDR.F32    s21,[r1,#4*4*0+4*1] // Matrix p1[0][1]
    VMUL.F32    s8,s12,s20
    VMUL.F32    s9,s13,s20
    VMUL.F32    s10,s14,s20
    VMLA.F32    s11,s15,s20
    VLDMIA      r2,{s12-s15}        // Save Matrix p2 into [S12-S15] regs

    VLDR.F32    s20,[r1,#4*4*1+4*1] // Matrix p1[1][1]
    VMLA.F32    s0,s16,s21
    VMLA.F32    s1,s17,s21
    VMLA.F32    s2,s18,s21
    VMLA.F32    s3,s19,s21

    VLDR.F32    s21,[r1,#4*4*2+4*1] // Matrix p1[2][1]
    VMLA.F32    s4,s16,s20
    VMLA.F32    s5,s17,s20
    VMLA.F32    s6,s18,s20
    VMLA.F32    s7,s19,s20

    VLDR.F32    s20,[r1,#4*4*0+4*2] // Matrix p1[0][2]
    VMLA.F32    s8,s16,s21
    VMLA.F32    s9,s17,s21
    VMLA.F32    s10,s18,s21
    VMLA.F32    s11,s19,s21

    VLDR.F32    s21,[r1,#4*4*1+4*2] // Matrix p1[1][2]
    VMLA.F32    s0,s12,s20
    VMLA.F32    s1,s13,s20
    VMLA.F32    s2,s14,s20
    VMLA.F32    s3,s15,s20

    VLDR.F32    s20,[r1,#4*4*2+4*2] // Matrix p1[2][2]
    VMLA.F32    s4,s12,s21
    VMLA.F32    s5,s13,s21
    VMLA.F32    s6,s14,s21
    VMLA.F32    s7,s15,s21

    VMLA.F32    s8,s12,s20
    VMLA.F32    s9,s13,s20
    VMLA.F32    s10,s14,s20
    VMLA.F32    s11,s15,s20

    VPOP        {d8-d10} // POP Floats off the stack
    MOV         r1,r0
    VSTMIA      r1!,{s0-s3} // Store result 1
    VSTMIA      r1,{s4-s11} // Store result 2
    BX          lr // Resturn
}

asm MTX34* MTX34MultAsm(MTX34*, const MTX34*, f32){
    VLDMIA      r1,{s2-s13} // Save Matrix p into [S1-S12] registers

    VMUL.F32    s2,s2,s0
    VMUL.F32    s3,s3,s0
    VMUL.F32    s4,s4,s0
    VMUL.F32    s5,s5,s0

    VMUL.F32    s6,s6,s0
    VMUL.F32    s7,s7,s0
    VMUL.F32    s8,s8,s0
    VMUL.F32    s9,s9,s0

    VMUL.F32    s10,s10,s0
    VMUL.F32    s11,s11,s0
    VMUL.F32    s12,s12,s0
    VMUL.F32    s13,s13,s0
    
    VSTMIA      r0,{s2-s13} // Store result
    BX          lr // Return
}

asm MTX34* MTX34AddAsm(MTX34*, const MTX34*, const MTX34*){
    VPUSH       {d8-d9} // Save registers
    VLDMIA      r2,{s0-s11} // Save the entire p2 matrix into [S0-S11] regs
    VLDMIA      r1!,{s12-s19} // Save Matrix p1 into [S12-S19] regs

    VADD.F32    s0,s12,s0
    VADD.F32    s1,s13,s1
    VADD.F32    s2,s14,s2

    VADD.F32    s3,s15,s3
    VADD.F32    s4,s16,s4
    VLDMIA      r1!,{s12-s15} // Continuation of p1
    VADD.F32    s5,s17,s5

    VADD.F32    s6,s18,s6
    VADD.F32    s7,s19,s7
    VADD.F32    s8,s12,s8

    VADD.F32    s9,s13,s9
    VADD.F32    s10,s14,s10
    VADD.F32    s11,s15,s11

    VPOP        {d8-d9} // POP Off the float stack; reg return
    VSTMIA      r0,{s0-s11} // Store result
    BX          lr // Return
}

asm MTX34* MTX34MultScaleAsm(MTX34* , const MTX34* , const VEC3* ){
    VLDMIA      r1,{s0-s11} // Save Matrix p into the [S0-S11] regs
    VLDMIA      r2,{s12-s14} // Save VEC3 into the [S12-S14] regs

    VMUL.F32    s0,s0,s12
    VMUL.F32    s1,s1,s13
    VMUL.F32    s2,s2,s14

    VMUL.F32    s4,s4,s12
    VMUL.F32    s5,s5,s13
    VMUL.F32    s6,s6,s14

    VMUL.F32    s8,s8,s12
    VMUL.F32    s9,s9,s13
    VMUL.F32    s10,s10,s14

    VSTMIA      r0,{s0-s11} // Store result
    BX          lr // Return
}

asm u32 MTX34InverseAsm(MTX34*, const MTX34* ){
    VLDMIA      r1,{s0-s2}
    ADD         r1,#4*4
    VLDMIA      r1,{s3-s5}
    ADD         r1,#4*4
    VLDMIA      r1,{s6-s8}

    VMUL.F32    s10,s0,s4
    VMUL.F32    s11,s1,s5
    VMUL.F32    s12,s2,s3
    VMUL.F32    s13,s6,s4
    VMUL.F32    s14,s3,s1
    VMUL.F32    s15,s0,s7

    VMUL.F32    s10,s10,s8
    VMUL.F32    s11,s11,s6
    VMUL.F32    s12,s12,s7

    VMLS.F32    s10,s13,s2
    VMLS.F32    s11,s14,s8
    VMLS.F32    s12,s15,s5

    VADD.F32    s10,s10,s11
    VLDR.F32    s15,=1.0
    VADD.F32    s10,s10,s12

    FMRS        r2,s10
    CMP         r2,#0x80000000
    CMPNE       r2,#0
    MOVEQ       r0,#0
    BXEQ        lr

    VPUSH       {d8-d12} // Save registers
    VDIV.F32    s15,s10

    VMUL.F32    s16,s4,s8
    VMUL.F32    s17,s1,s8
    VMUL.F32    s18,s1,s5

    VMUL.F32    s19,s3,s8
    VMUL.F32    s20,s0,s8
    VMUL.F32    s21,s0,s5

    VMUL.F32    s22,s3,s7
    VMUL.F32    s23,s0,s7
    VMUL.F32    s24,s0,s4


    VMLS.F32    s16,s7,s5
    VMLS.F32    s17,s7,s2
    VMLS.F32    s18,s4,s2

    VMLS.F32    s19,s6,s5
    VMLS.F32    s20,s6,s2
    VMLS.F32    s21,s3,s2

    VMLS.F32    s22,s6,s4
    VMLS.F32    s23,s6,s1
    VMLS.F32    s24,s3,s1


    VMUL.F32    s0,s16,s15 // m[0][0]
    VNMUL.F32   s1,s17,s15 // m[0][1]
    VMUL.F32    s2,s18,s15 // m[0][2]

    VNMUL.F32   s4,s19,s15// m[1][0]
    VMUL.F32    s5,s20,s15 // m[1][1]
    VNMUL.F32   s6,s21,s15 // m[1][2]

    VMUL.F32    s8,s22,s15 // m[2][0]
    VLDR.F32    s12,[r1,#4*3-(4*4*2)]// src[0][3]
    VNMUL.F32   s9,s23,s15 // m[2][1]
    VMUL.F32    s10,s24,s15 // m[2][2]

    VNMUL.F32   s3,s0,s12
    VLDR.F32    s13,[r1,#4*3-(4*4)] // src[1][3]
    VNMUL.F32   s7,s4,s12
    VNMUL.F32   s11,s8,s12

    VMLS.F32    s3,s1,s13
    VLDR.F32    s14,[r1,#4*3] // src[2][3]
    VMLS.F32    s7,s5,s13
    VMLS.F32    s11,s9,s13

    VMLS.F32    s3,s2,s14
    VMLS.F32    s7,s6,s14
    VMLS.F32    s11,s10,s14

    VPOP        {d8-d12} // POP off the Float Stack; reg return
    VSTMIA      r0,{s0-s11} // Store result
    MOV         r0,#1
    BX          lr // Return
}

asm u32 MTX34InvTransposeAsm(MTX34*, const MTX34* ){
    VLDMIA      r1,{s0-s2}
    ADD         r1,#4*4
    VLDMIA      r1,{s3-s5}
    ADD         r1,#4*4
    VLDMIA      r1,{s6-s8}

    VMUL.F32    s10,s0,s4
    VMUL.F32    s11,s1,s5
    VMUL.F32    s12,s2,s3
    VMUL.F32    s13,s6,s4
    VMUL.F32    s14,s3,s1
    VMUL.F32    s15,s0,s7

    VMUL.F32    s10,s10,s8
    VMUL.F32    s11,s11,s6
    VMUL.F32    s12,s12,s7

    VMLS.F32    s10,s13,s2
    VMLS.F32    s11,s14,s8
    VMLS.F32    s12,s15,s5

    VADD.F32    s10,s10,s11
    VLDR.F32    s15,=1.0
    VADD.F32    s10,s10,s12

    FMRS        r2,s10
    CMP         r2,#0x80000000
    CMPNE       r2,#0
    MOVEQ       r0,#0
    BXEQ        lr

    VPUSH       {d8-d12} // Save registers
    VDIV.F32    s15,s10

    VMUL.F32    s16,s4,s8
    VMUL.F32    s17,s3,s8
    VMUL.F32    s18,s3,s7

    VMUL.F32    s19,s1,s8
    VMUL.F32    s20,s0,s8
    VMUL.F32    s21,s0,s7

    VMUL.F32    s22,s1,s5
    VMUL.F32    s23,s0,s5
    VMUL.F32    s24,s0,s4


    VMLS.F32    s16,s7,s5
    VMLS.F32    s17,s6,s5
    VMLS.F32    s18,s6,s4

    VMLS.F32    s19,s7,s2
    VMLS.F32    s20,s6,s2
    VMLS.F32    s21,s6,s1

    VMLS.F32    s22,s4,s2
    VMLS.F32    s23,s3,s2
    VMLS.F32    s24,s3,s1


    VMUL.F32    s0,s16,s15 // m[0][0]
    VNMUL.F32   s1,s17,s15 // m[0][1]
    VMUL.F32    s2,s18,s15 // m[0][2]

    VNMUL.F32   s4,s19,s15 // m[1][0]
    VMUL.F32    s5,s20,s15 // m[1][1]
    VNMUL.F32   s6,s21,s15 // m[1][2]

    VMUL.F32    s8,s22,s15 // m[2][0]
    VNMUL.F32   s9,s23,s15 // m[2][1]
    VMUL.F32    s10,s24,s15 // m[2][2]

    VLDR.F32    s3,=0.0
    VLDR.F32    s7,=0.0
    VLDR.F32    s11,=0.0

    VPOP        {d8-d12} // POP off the Float Stack; reg return
    VSTMIA      r0,{s0-s11} // Store result
    MOV         r0,#1
    BX          lr // Return

}

asm MTX34* MTX34MultTranslateAsm(MTX34*, const VEC3*, const MTX34*){
    VLDMIA      r2,{s0-s11} // The entire pM matrix is put in the [S0-S11] registers
    VLDMIA      r1,{s12-s14} // All vectors are put in the [S0-S11] registers
    
    VADD.F32    s3,s3,s12
    VADD.F32    s7,s7,s13
    VADD.F32    s11,s11,s14

    VSTMIA      r0,{s0-s11} // Store result
    BX          lr // Return
}

asm MTX34* MTX34MultTranslateAsm(MTX34*, const MTX34*, const VEC3*){
    VLDMIA      r1,{s0-s11} // Save the entire pM matrix is put in the [S0-S11] registers
    VLDMIA      r2,{s12-s14} // Save all vectors are put in the [S0-S11] registers

    VMLA.F32    s3,s0,s12
    VMLA.F32    s7,s4,s12
    VMLA.F32    s11,s8,s12
    
    VMLA.F32    s3,s1,s13
    VMLA.F32    s7,s5,s13
    VMLA.F32    s11,s9,s13

    VMLA.F32    s3,s2,s14
    VMLA.F32    s7,s6,s14
    VMLA.F32    s11,s10,s14

    VSTMIA      r0,{s0-s11} // Store result
    BX          lr // Return

}

asm VEC3* VEC3TransformAsm(VEC3* , const MTX34* , const VEC3* ){
    VLDMIA      r1,{s0-s11} // Save pM into [S0-S11] regs
    VLDMIA      r2,{s12-s14} // Save all vecs into [S12-S14] regs

    VMLA.F32    s3,s0,s12
    VMLA.F32    s7,s4,s12
    VMLA.F32    s11,s8,s12
    
    VMLA.F32    s3,s1,s13
    VMLA.F32    s7,s5,s13
    VMLA.F32    s11,s9,s13

    VMLA.F32    s3,s2,s14
    VMLA.F32    s7,s6,s14
    VMLA.F32    s11,s10,s14

    VSTR.F32    s3,[r0,#0]
    VSTR.F32    s7,[r0,#4]
    VSTR.F32    s11,[r0,#8]
    BX          lr // Return
}

asm MTX34* MTX34TransposeAsm(MTX34* , const MTX34* ){
    VLDR.F32    s0,[r1,#0*16+0*4]
    VLDR.F32    s1,[r1,#1*16+0*4]
    VLDR.F32    s2,[r1,#2*16+0*4]
    VLDR.F32    s3,=0.0
    VLDR.F32    s4,[r1,#0*16+1*4]
    VLDR.F32    s5,[r1,#1*16+1*4]
    VLDR.F32    s6,[r1,#2*16+1*4]
    VLDR.F32    s7,=0.0
    VLDR.F32    s8,[r1,#0*16+2*4]
    VLDR.F32    s9,[r1,#1*16+2*4]
    VLDR.F32    s10,[r1,#2*16+2*4]
    VLDR.F32    s11,=0.0

    VSTMIA      r0,{s0-s11} // Store result
    BX          lr // Return
}

}
}
}
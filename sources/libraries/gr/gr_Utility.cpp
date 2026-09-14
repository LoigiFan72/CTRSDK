// Filename: gr_Utility.cpp
//
// Project: Horizon

namespace nn  {
namespace gr  {
namespace CTR {

asm void CopyMtx34WithHeader(f32* /* dst */, const nn::math::MTX34* /* src */, u32 /* header */)
{
    VLDMIA      r1!,{s0-s11}
    STR         r2,[r0,#4]
    VSTR.F32    s0,[r0,#16]
    VSTR.F32    s1,[r0,#12]
    VSTR.F32    s2,[r0,#8]
    VSTR.F32    s3,[r0,#0]
    VSTR.F32    s4,[r0,#32]
    VSTR.F32    s5,[r0,#28]
    VSTR.F32    s6,[r0,#24]
    VSTR.F32    s7,[r0,#20]
    VSTR.F32    s8,[r0,#48]
    VSTR.F32    s9,[r0,#44]
    VSTR.F32    s10,[r0,#40]
    VSTR.F32    s11,[r0,#36]
    BX      lr
}

asm void CopyMtx44WithHeader(f32* /* dst */, const nn::math::MTX44* /* src */, u32 /* header */)
{
    VLDMIA      r1!,{s0-s11}
    STR         r2,[r0,#4]
    VSTR.F32    s0,[r0,#16]
    VSTR.F32    s1,[r0,#12]
    VSTR.F32    s2,[r0,#8]
    VSTR.F32    s3,[r0,#0]
    VSTR.F32    s4,[r0,#32]
    VSTR.F32    s5,[r0,#28]
    VSTR.F32    s6,[r0,#24]
    VSTR.F32    s7,[r0,#20]
    VSTR.F32    s8,[r0,#48]
    VSTR.F32    s9,[r0,#44]
    VSTR.F32    s10,[r0,#40]
    VSTR.F32    s11,[r0,#36]
    VSTR.F32    s12,[r0,#64]
    VSTR.F32    s13,[r0,#60]
    VSTR.F32    s14,[r0,#56]
    VSTR.F32    s15,[r0,#52]
    BX      lr
}
}
}
}
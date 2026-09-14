#pragma once

#include <nn/types.h>
#include <nn/Assert.h>
#include <nn/math.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>
#include <nn/gx.h>
#include <nn/gx/CTR/gx_PicaAll.h>


namespace nn{
namespace gr{
namespace CTR{

void CopyMtx44WithHeader(f32* dst, const nn::math::MTX44* src, bit32 header);
void CopyMtx34WithHeader(f32* dst, const nn::math::MTX34* src, bit32 header);

inline u32 PicaDataVertexAttrTypeToByteSize(const PicaDataVertexAttrType type)
{
    switch (type)
    {
        case PICA_DATA_SIZE_1_BYTE          : return 1 * sizeof(s8);
        case PICA_DATA_SIZE_1_UNSIGNED_BYTE : return 1 * sizeof(u8);
        case PICA_DATA_SIZE_1_SHORT         : return 1 * sizeof(s16);
        case PICA_DATA_SIZE_1_FLOAT         : return 1 * sizeof(f32);
        case PICA_DATA_SIZE_2_BYTE          : return 2 * sizeof(s8);
        case PICA_DATA_SIZE_2_UNSIGNED_BYTE : return 2 * sizeof(u8);
        case PICA_DATA_SIZE_2_SHORT         : return 2 * sizeof(s16);
        case PICA_DATA_SIZE_2_FLOAT         : return 2 * sizeof(f32);
        case PICA_DATA_SIZE_3_BYTE          : return 3 * sizeof(s8);
        case PICA_DATA_SIZE_3_UNSIGNED_BYTE : return 3 * sizeof(u8);
        case PICA_DATA_SIZE_3_SHORT         : return 3 * sizeof(s16);
        case PICA_DATA_SIZE_3_FLOAT         : return 3 * sizeof(f32);
        case PICA_DATA_SIZE_4_BYTE          : return 4 * sizeof(s8);
        case PICA_DATA_SIZE_4_UNSIGNED_BYTE : return 4 * sizeof(u8);
        case PICA_DATA_SIZE_4_SHORT         : return 4 * sizeof(s16);
        case PICA_DATA_SIZE_4_FLOAT         : return 4 * sizeof(f32);
    }
    return 0;
}

inline u8 FloatToUnsignedByte(f32 val)
{
    return (u8)(0.5f + (val <0.f ? 0.f : (1.f < val ? 1.f : val)) * (0xff));
}

inline u32 Float32ToUnsignedFix24(f32 val)
{
    unsigned v_ = *(reinterpret_cast<unsigned*>(&val));

    if (val <= 0 || (v_ & 0x7f800000) == 0x7f800000)
    {
        return 0;
    }

    else
    {
        val *= 1 << (24 - 0);

        if (val >= (1 << 24))
        {
            return (1 << 24) - 1;
        }
        else
        {
            return (unsigned)(val);
        }
    }
}

inline u16 Float32ToFloat16(f32 val)
{
    static const int bias_ = 128 - (1 << (5 - 1));

    u32 uval_ = *(reinterpret_cast<u32*>(&val));
    int e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;
    if (e_ >= 0)
    {
        return (u16)( ((uval_ & 0x7fffff) >> (23 - 10)) | (e_ << 10) | ((uval_ >> 31) << (10 + 5)));
    }

    return (u16)((uval_ >> 31) << (10 + 5));
}

inline u32 Float32ToFloat24(f32 val)
{
    static const int bias_ = 128 - (1 << (7 - 1));
    u32 uval_   = *( reinterpret_cast<unsigned*>( &val ) );
    s32 e_      = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;

    return e_ >= 0 ? ((uval_ & 0x7fffff) >> (23 - 16)) | (e_ << 16) | ((uval_ >> 31) << (16 + 7)) : ((uval_ >> 31) << (16 + 7));
}

inline bit32* MakeUniformCommandVS(bit32* command, u8 location, const nn::math::MTX34& mtx34)
{
    *command++ = 0x80000000 | location;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_VS_FLOAT_ADDR);
    CopyMtx34WithHeader(reinterpret_cast<f32*>(command), &mtx34, PICA_CMD_HEADER_VS_F32(3));
    return command + 14;
}

inline bit32* MakeUniformCommandVS(bit32* command, u8 location, const nn::math::MTX44& mtx44)
{
    *command++ = 0x80000000 | location;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_VS_FLOAT_ADDR);
    CopyMtx44WithHeader(reinterpret_cast<f32*>(command), &mtx44, PICA_CMD_HEADER_VS_F32(4));
    return command + 18;
}

inline bit32* MakeUniformCommandVS(bit32* command, u8 location, const nn::math::VEC4& vec4)
{
    *command++ = 0x80000000 | location;
    *command++ = PICA_CMD_HEADER_BURSTSEQ(PICA_REG_VS_FLOAT_ADDR, 5);

    *command++ = nn::math::F32AsU32(vec4.w);
    *command++ = nn::math::F32AsU32(vec4.z);
    *command++ = nn::math::F32AsU32(vec4.y);
    *command++ = nn::math::F32AsU32(vec4.x);
    return command;
}

inline bit32* MakeUniformCommandGS(bit32* command, u8 location, const nn::math::VEC4& vec4)
{
    *command++ = 0x80000000 | location;
    *command++ = PICA_CMD_HEADER_BURSTSEQ(PICA_REG_GS_FLOAT_ADDR, 5);
    *command++ = nn::math::F32AsU32(vec4.w);
    *command++ = nn::math::F32AsU32(vec4.z);
    *command++ = nn::math::F32AsU32(vec4.y);
    *command++ = nn::math::F32AsU32(vec4.x);
    return command;
}

}
}
}
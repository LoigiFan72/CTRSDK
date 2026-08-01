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

void CopyMtx44WithHeader(f32* dst,const nn::math::MTX44* src,bit32 header);

inline u32 Float32ToUnsignedFix24( f32 val ){
    unsigned v_ = *(reinterpret_cast<unsigned*>(&val));

    if (val <= 0 || (v_ & 0x7f800000) == 0x7f800000){
        return 0;
    }

    else{
        val *= 1 << (24 - 0);

        if (val >= (1 << 24)){
            return (1 << 24) - 1;
        }

        else{
            return (unsigned)(val);
        }
    }
}

inline u16 Float32ToFloat16(f32 val){
    static const int bias_ = 128 - (1 << (5 - 1));

    u32 uval_ = *(reinterpret_cast<u32*>(&val));
    int e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;
    if (e_ >= 0){
        return (u16)( ((uval_ & 0x7fffff) >> (23 - 10)) | (e_ << 10) | ((uval_ >> 31) << (10 + 5)));
    }

    return (u16)((uval_ >> 31) << (10 + 5));
}

inline u32 Float32ToFloat24(f32 val){
    static const int bias_ = 128 - (1 << (7 - 1));
    u32 uval_   = *( reinterpret_cast<unsigned*>( &val ) );
    s32 e_      = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0;

    return e_ >= 0 ? ((uval_ & 0x7fffff) >> (23 - 16)) | (e_ << 16) | ((uval_ >> 31) << (16 + 7)) : ((uval_ >> 31) << (16 + 7));
}

}
}
}
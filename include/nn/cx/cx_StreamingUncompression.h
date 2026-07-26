#pragma once

#include <nn/types.h>

namespace nn{
namespace cx{

struct UncompContextLZ{
    u8 *destp;
    s32 destCount;
    s32 forceDestCount;
    s32 length;
    u8 lengthFlg;
    u8 flags;
    u8 flagIndex;
    u8 headerSize;
    u8 exFormat;
    s8 rev[3];
};

void InitUncompContextLZ(UncompContextLZ *context, void *dest);
s32 ReadUncompLZ(UncompContextLZ *context, const void *data, u32 len);

}
}
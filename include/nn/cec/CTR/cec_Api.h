#pragma once

#include <nn/types.h>
#include <nn/cec/cec_Result.h>
#include <nn/cec/CTR/cec_Cec.h>
#include <nn/cec/CTR/cec_CecAPI.h>
#include <nn/cec/CTR/cec_Message.h>
#include <nn/cec/CTR/cec_MessageBox.h>
#include <nn/cec/CTR/cec_Control.h>

namespace nn{
namespace cec{
namespace CTR{

typedef enum CecBoxDataType{
    BOXDATA_TYPE_START =  100,
    BOXDATA_TYPE_ICON,
    BOXDATA_TYPE_NAME_1 = 110,
    BOXDATA_TYPE_END =    200
}BoxDataType;

u32 Base64Str2CecTitleId(const u8* str);
void SetAllocFunc(nn::fnd::IAllocator& cecAllocFunc);

}
}
}
#pragma once

#include <nn/hid/CTR/hid_Pad.h>
#include <nn/hid/CTR/hid_ExtraPadStatus.h>

namespace nn{
namespace hidlow{

short ClampStickCircle(short *pOutX,short *pOutY,s32 x,s32 y,s32 min,s32 max);
short ClampStickCross(short *pOutX,short *pOutY,s32 x,s32 y,s32 min,s32 max);
f32 ClampStickCrossFloat(f32 *pOutX,f32 *pOutY,s32 x,s32 y,s32 min,s32 max);
void ClampStickMinimum(s16 *pOutX,s16 *pOutY,s32 x,s32 y,s32 min,s32 max);

void GatherStartAndSelect(hid::CTR::ExtraPadStatus* pButton);
void GatherStartAndSelect(hid::CTR::PadStatus* pButton);
void GatherStartAndSelect(bit32& hold,bit32& trigger,bit32& release);

}
}
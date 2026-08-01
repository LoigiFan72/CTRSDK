// Filename: hidlow_Utils.cpp
//
// Project: Horizon

#include <math.h>
#include <nn/assert.h>
#include <nn/math.h>
#include <nn/Result.h>
#include <nn/hidlow/hidlow_Utils.h>
#include <nn/hid/CTR/hid_AnalogStickClamper.h>

namespace nn{
namespace hidlow{

s32 sqrti(s32 x){
  if (0 >= x){
      return 0;
  }

  s32 tmp  = 1;
  s32 root = x;

  while (tmp < root){
    tmp  <<= 1;
    root >>= 1;
  }

  do{
    root = tmp;
    tmp = (x / tmp + tmp) >> 1;
  } while (tmp < root);

  return root;
}

short ClampStickCircle(short *pOutX,short *pOutY,s32 x,s32 y,s32 min,s32 max){
    s32 length2 = x * x + y * y;
    s32 seg;
    s32 scalar = 0;

    if (length2 <= min*min){
        *pOutX = *pOutY = 0;
        return scalar;
    }

    s32 length = sqrti(length2 << 7 *2);
    if (length2 >= max*max){
        scalar = max - min;
        seg = scalar << 7;
    }

    else{
        seg = length - (min << 7);
        scalar = (length >> 7) - min;
    }

    *pOutX = (s16)(x * seg / length);
    *pOutY = (s16)(y * seg / length);
    return scalar;
}

short ClampStickCross(short *pOutX,short *pOutY,s32 x,s32 y,s32 min,s32 max){
    if (x < 0){
        x =  -x <= min ? 0 : x + min;
    }
    else{
        x =  x <= min ? 0 : x - min;
    }

    if (y < 0){
        y =  -y <= min ? 0 : y + min;
    }
    else{
        y =  y <= min ? 0 : y - min;
    }

	s32 length2 = x * x + y * y;
	s32 scalar = max - min;

	if (length2 >= (max-min)*(max-min)){
	    s32 length = sqrti(length2 << 7 *2);

        s32 seg = scalar << 7;

	    x = (s16)(x * seg / length);
	    y = (s16)(y * seg / length);
    }

    *pOutX = (s16)x;
    *pOutY = (s16)y;
    return scalar;
}

f32 ClampAxisFloat(s32 x, s32 min, s32 max){
    if (x <= min){
       return 0.f;
    }
    else if (x >= max ){
        return 1.f;
    }
    else{
        return (f32)(x - min) / (f32)(max - min);
    }
}

f32 ClampStickCrossFloat(f32 *pOutX,f32 *pOutY,s32 x,s32 y,s32 min,s32 max){
    f32 length;

    if (x < 0){
        *pOutX = - ClampAxisFloat(-x, min, max);
    }
    else{
        *pOutX = ClampAxisFloat(x, min, max);
    }

    if (y < 0){
        *pOutY = - ClampAxisFloat(-y, min, max);
    }
    else{
        *pOutY = ClampAxisFloat(y, min, max);
    }

    length = (*pOutX) * (*pOutX) + (*pOutY) * (*pOutY);
    length = sqrtf(length);
    if (length > 1.f) {
        (*pOutX) /= length ;
        (*pOutY) /= length ;
        length = 1.f;
    }
    return length;
}

void ClampStickMinimum(s16* pOutX, s16* pOutY, s32 x, s32 y, s32 min, s32 max){
	NN_UNUSED_VAR(min);
	f32 cross_x, cross_y;
	f32 len_cross_2;
	f32 len_circle_2;
    f32 len_min;
	f32 len_min_2;
	f32 len_out_max_2;
	f32 len_input;
	f32 len_input_2;
	f32 A,B;
	f32 max_value;

	len_input_2 = x*x + y*y;

	if (len_input_2 == 0.0f){
		*pOutX = 0;
		*pOutY = 0;
		return;
	}

	if (x * x > y * y){
		if (x < 0){
			cross_x = -nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CROSS;
			cross_y = cross_x * y / x;
		}
		else{
			cross_x = nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CROSS;
			cross_y = cross_x * y / x;
		}
	}
	else{
		if (y < 0){
			cross_y = -nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CROSS;
			cross_x = cross_y * x / y;
		}
		else{
			cross_y = nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CROSS;
			cross_x = cross_y * x / y;
		}
	}

	len_cross_2 = cross_x * cross_x + cross_y * cross_y;

	len_circle_2 = (f32)nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CIRCLE * nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CIRCLE;

    len_min_2 = (len_cross_2 < len_circle_2)? len_cross_2	: len_circle_2;

    if (len_input_2 < len_min_2){
		*pOutX = 0;
		*pOutY = 0;
		return;
	}

    len_out_max_2 = max * max;
    len_min = nn::math::FSqrt(len_min_2);

	len_input = nn::math::FSqrt(len_input_2);

    A = len_input - len_min;

    B = max - len_min;

    max_value = max - nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CIRCLE;

    if (len_input_2 >= len_out_max_2){
        *pOutX = x * max_value / len_input;
        *pOutY = y * max_value / len_input;
    }

    else{
        *pOutX = x * max_value * A / (B * len_input);
        *pOutY = y * max_value * A / (B * len_input);
    }
}


void GatherStartAndSelect(hid::CTR::PadStatus* pButton){
    GatherStartAndSelect(pButton->hold,pButton->trigger,pButton->release);
}

void GatherStartAndSelect(hid::CTR::ExtraPadStatus* pButton){
    GatherStartAndSelect(pButton->hold,pButton->trigger,pButton->release);
}

void GatherStartAndSelect(bit32& hold,bit32& trigger,bit32& release){
    if (((nn::hid::CTR::PSEUDO_BUTTON_SELECT | nn::hid::CTR::BUTTON_START) == (trigger & (nn::hid::CTR::PSEUDO_BUTTON_SELECT | nn::hid::CTR::BUTTON_START)))
        ||  ((0 != (trigger & nn::hid::CTR::PSEUDO_BUTTON_SELECT)) && (0 == (hold & nn::hid::CTR::BUTTON_START)))
        ||  ((0 != (trigger & nn::hid::CTR::BUTTON_START)) && (0 == (hold & nn::hid::CTR::PSEUDO_BUTTON_SELECT)))){
        trigger |= nn::hid::CTR::BUTTON_START;
    }
    else{
        trigger &= ~nn::hid::CTR::BUTTON_START;
    }

    if (((nn::hid::CTR::PSEUDO_BUTTON_SELECT | nn::hid::CTR::BUTTON_START) == (release & (nn::hid::CTR::PSEUDO_BUTTON_SELECT | nn::hid::CTR::BUTTON_START)))
        || ((0 != (release & nn::hid::CTR::PSEUDO_BUTTON_SELECT)) && (0 == (hold & nn::hid::CTR::BUTTON_START)))
        || ((0 != (release & nn::hid::CTR::BUTTON_START)) && (0 == (hold & nn::hid::CTR::PSEUDO_BUTTON_SELECT)))){
        release |= nn::hid::CTR::BUTTON_START;
    }
    else{
        release &= ~nn::hid::CTR::BUTTON_START;
    }

    if (0 != (hold & nn::hid::CTR::PSEUDO_BUTTON_SELECT)){
        hold = (hold & ~nn::hid::CTR::PSEUDO_BUTTON_SELECT) | nn::hid::CTR::BUTTON_START;
    }

    trigger &= ~nn::hid::CTR::PSEUDO_BUTTON_SELECT;
    release &= ~nn::hid::CTR::PSEUDO_BUTTON_SELECT;
}

}
}
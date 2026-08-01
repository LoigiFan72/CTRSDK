#pragma once

#include <nn/types.h>

namespace nn {
namespace codec {
namespace CTR {
        enum IirFilterTargetEQ{
                HP_32KHZ = 0,
                HP_48KHZ = 1,
                SP_32KHZ = 2,
                SP_48KHZ = 3
        };

        typedef struct{
                u16 n0;
                u16 n1;
                u16 d1;
        } IirFilterParamHalf;

        typedef struct{
                u16 n0;
                u16 n1;
                u16 n2;
                u16 d1;
                u16 d2;
        } IirFilterParamBiquad;

        typedef struct{
                IirFilterParamHalf   half;
                IirFilterParamBiquad biquadA;
                IirFilterParamBiquad biquadB;
                IirFilterParamBiquad biquadC;
                IirFilterParamBiquad biquadD;
                IirFilterParamBiquad biquadE;
        } IirFilterParam;

        typedef struct{
                IirFilterParamBiquad biquadA;
                IirFilterParamBiquad biquadB;
                IirFilterParamBiquad biquadC;
        } IirFilterParamEQ;

        typedef struct{
                IirFilterParamBiquad biquadA;
                IirFilterParamBiquad biquadB;
                IirFilterParamBiquad biquadC;
                IirFilterParamBiquad biquadD;
                IirFilterParamBiquad biquadE;
        } IirFilterParamMic;

}
}
}
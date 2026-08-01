#pragma once

namespace nn {
namespace mic {
namespace CTR {

    enum SamplingType{
        SAMPLING_TYPE_8BIT = 0,
        SAMPLING_TYPE_16BIT = 1,
        SAMPLING_TYPE_SIGNED_8BIT = 2,
        SAMPLING_TYPE_SIGNED_16BIT = 3
    };
    
    enum SamplingRate{
        SAMPLING_RATE_32730 = 0,
        SAMPLING_RATE_16360 = 1,
        SAMPLING_RATE_10910 = 2,
        SAMPLING_RATE_8180 = 3
    };

    typedef struct {
        s32 offset;
    } Header;
}
}
}
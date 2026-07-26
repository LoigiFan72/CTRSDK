#pragma once

#include "nn/types.h"

namespace nn {
namespace ro {

    enum FixLevel{
        FIX_LEVEL_0,
        FIX_LEVEL_1,
        FIX_LEVEL_2,
        FIX_LEVEL_3,
        FIX_LEVEL_NONE = FIX_LEVEL_0, 
        FIX_LEVEL_INTERNAL = FIX_LEVEL_1,
        FIX_LEVEL_INTERNAL_IMPORT = FIX_LEVEL_2,
        FIX_LEVEL_INTERNAL_IMPORT_EXPORT = FIX_LEVEL_3,
        FIX_LEVEL_MAX_BITS = (1u << 31)
    };

    struct SizeInfo{
        uptr    mFix0End;
        uptr    mFix1End;
        uptr    mFix2End;
        uptr    mFix3End;
        size_t  mBufferSize;
    };

    struct RegionInfo{
        uptr    mMapBegin;
        size_t  mMapSize;
        uptr    mCroBegin;
        size_t  mCroSize; 
        uptr    mDataBssBegin;
        size_t  mDataBssSize;
        uptr    mCodeBegin;
        size_t  mCodeSize;
    };

} // end of namespace ro
} // end of namespace nn
// Filename: fslow_QueryOptimalBucketCount.cpp
//
// Project: Horizon

#include <nn/fslow/fslow_QueryOptimalBucketCount.h>

namespace nn {
namespace fslow {

u32 QueryOptimalBucketCount(u32 countEntries){
    if (countEntries <= 3){
        return 3;
    }
    if (countEntries <= 19){
        return countEntries | 1;
    }

    u32 i;
    for (i = 0; i < 100; i++){
        u32 candidate = (countEntries + i);
        if ((candidate % 2) != 0 && (candidate % 3) != 0 && (candidate % 5) != 0 && (candidate % 7) != 0 && (candidate % 11) != 0 && (candidate % 13) != 0 && (candidate % 17) != 0){
            return candidate;
        }
    }
    return countEntries | 1;
}

}
}
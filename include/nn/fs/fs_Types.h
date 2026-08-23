#pragma once

#include <nn/types.h>

namespace nn{
namespace fs{
    struct ArchiveResource{
        uint mSectorSize;
        uint mClusterSize;
        uint mTotalClusters;
        uint mFreeClusters;
    };

    enum CardType{
        TYPE_CTR = 0,
        TYPE_DS,
        TYPE_UNKNOWN,
    };

    enum CardSpiBusMode{
        BIT_1 = 0,
        BIT_4 = 3,
    };

    enum CardSpiBaudRate{
        KHZ_512 = 0,
        MHZ_1,
        MHZ_2,
        MHZ_4,
        MHZ_8,
        MHZ_16,
    };

    struct SdmcLog{
    };

    struct SdmcSpeedInfo{
        bool mCardHighSpeed;
        bool mSdIfMaxSpeed;
        ushort mSdIfCtrlValue;
    };

    struct CtrCardCompatibilityInfo{
        u64 mCompatibilityInfo;
    };

    struct CtrCardLatencyParamater{
        u64 mLatencyInfo;
    };

    struct Fs9CompatibilityInfo : public CtrCardCompatibilityInfo{
    };

    struct FsCompatibilityInfo : public Fs9CompatibilityInfo{
    };

}
}
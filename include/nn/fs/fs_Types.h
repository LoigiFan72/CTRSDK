#pragma once

#include <nn/types.h>

namespace nn{
namespace fs{
    struct ArchiveResource
    {
        uint mSectorSize;
        uint mClusterSize;
        uint mTotalClusters;
        uint mFreeClusters;
    };

    enum CardType
    {
        TYPE_CTR = 0,
        TYPE_DS,
        TYPE_UNKNOWN,
    };

    enum CardSpiBusMode
    {
        BIT_1 = 0,
        BIT_4 = 3,
    };

    enum CardSpiBaudRate
    {
        KHZ_512 = 0,
        MHZ_1,
        MHZ_2,
        MHZ_4,
        MHZ_8,
        MHZ_16,
    };

    struct SdmcLog
    {
    };

    struct SdmcSpeedInfo
    {
        bool cardHighSpeed;
        bool sdIfMaxSpeed;
        ushort sdIfCtrlValue;
    };

    struct CtrCardCompatibilityInfo
    {
        u64 compatibilityInfo;
    };

    struct CtrCardLatencyParamater{
        u64 latencyInfo;
    };

    struct Fs9CompatibilityInfo : public CtrCardCompatibilityInfo{
    };

    struct FsCompatibilityInfo : public Fs9CompatibilityInfo{
    };

}
}
// Filename: ptm_Rtc.cpp
//
// Project: Horizon

#include <nn/ptm/CTR/ptm_Rtc.h>
#include <nn/fnd/fnd_DateTime.h>
#include <nn/os/ARM/os_MemoryBarrier.h>
#include <nn/os.h>
#include <algorithm>

namespace nn{
namespace ptm{
namespace CTR{
namespace detail{

void GetSwcInfo(nn::os::Tick *pNowTicks, s64 *pBaseMilliSeconds, nn::os::Tick *pBaseTicks, s64 *pTicksPerSecond, s64 *pDiffMilliSeconds){
    int currentNum;
    do {
        *pNowTicks = nn::os::Tick::GetSystemCurrent();

        currentNum = nn::os::GetWritableSharedInfo().rtcValidNumber;
        nn::os::RtcSwcInfo& swcInfo = nn::os::GetWritableSharedInfo().rtcSwcInfo[currentNum % 2 == 0 ? 0 : 1];
                
        *pBaseMilliSeconds = swcInfo.rtcBaseMilliSeconds;
        *pBaseTicks = swcInfo.rtcBaseTicks;
        *pTicksPerSecond = swcInfo.rtcTicksPerSecond;
        *pDiffMilliSeconds = swcInfo.rtcDiffMilliSeconds;
            
        nn::os::ARM::DataMemoryBarrier();

    } while(currentNum != nn::os::GetWritableSharedInfo().rtcValidNumber);
}

s64 GetSwcMilliSeconds(){
    const s64 EPOC = (nn::fnd::DateTime(2000,1,1) - nn::fnd::DateTime::MIN_DATETIME).GetMilliSeconds();
    const s64 OVERLAP = (nn::fnd::DateTime(2100,1,1) - nn::fnd::DateTime(2000,1,1)).GetMilliSeconds();
    nn::os::Tick nowTicks, baseTicks;
    s64 baseMSec, ticksPerSec, diffMSec;

    GetSwcInfo(&nowTicks, &baseMSec, &baseTicks, &ticksPerSec, &diffMSec);

    s64 nowMSec = baseMSec + nnmathMultiplyAndDivide((static_cast<s64>(nowTicks)-static_cast<s64>(baseTicks)),1000,ticksPerSec);
    s64 restMSec = std::max(0LL, nn::ptm::CTR::detail::ERROR_MODIFIED_INTERVAL - (nowMSec - baseMSec));
    return (nowMSec + nnmathMultiplyAndDivide(diffMSec,restMSec,nn::ptm::CTR::detail::ERROR_MODIFIED_INTERVAL) - EPOC) % OVERLAP + EPOC;
}

}
}
}
}
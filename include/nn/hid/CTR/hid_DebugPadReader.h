#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_Api.h>
#include <nn/hid/CTR/hid_Pad.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/hid/CTR/hid_Api.h>

namespace nn {
namespace hid {
namespace CTR {

class DebugPadReader : private nn::util::ADLFireWall::NonCopyable<DebugPadReader>{
public:
    typedef enum{
        STICK_CLAMP_MODE_CIRCLE_WITH_PLAY = 0,
        STICK_CLAMP_MODE_CIRCLE_WITHOUT_PLAY
    } StickClampMode;

    DebugPadReader(DebugPad& debugPad=GetDebugPad( )): 
        mDebugPad(debugPad), 
        mIndexOfRead(-1), 
        mIsReadLatestFirst(true), 
        mStickClampMode(STICK_CLAMP_MODE_CIRCLE_WITH_PLAY), 
        mTickOfRead(-1)
    {};
    ~DebugPadReader( ) {};
    bool ReadLatest(DebugPadStatus* pBuf);
    void Read(DebugPadStatus* pBufs, s32* pReadLen, s32 bufLen);

    StickClampMode GetStickClampMode() const;
    void SetStickClampMode(StickClampMode mode);
protected:
    DebugPad& mDebugPad;
    s32 mIndexOfRead;
    bit16 mLatestHold;
    bool mIsReadLatestFirst;
    nn::util::SizedEnum1<StickClampMode> mStickClampMode;
    s32 pad;
    s64 mTickOfRead;
};

inline DebugPadReader::StickClampMode DebugPadReader::GetStickClampMode( ) const{
    return mStickClampMode;
}

inline void DebugPadReader::SetStickClampMode(DebugPadReader::StickClampMode mode){
    mStickClampMode = mode;
}

}
}
}
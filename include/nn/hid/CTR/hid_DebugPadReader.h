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
        m_DebugPad(debugPad), 
        m_IndexOfRead(-1), 
        m_IsReadLatestFirst(true), 
        m_StickClampMode(STICK_CLAMP_MODE_CIRCLE_WITH_PLAY), 
        m_TickOfRead(-1)
    {};
    ~DebugPadReader( ) {};
    bool ReadLatest(DebugPadStatus* pBuf);
    void Read(DebugPadStatus* pBufs, s32* pReadLen, s32 bufLen);

    StickClampMode GetStickClampMode() const;
    void SetStickClampMode(StickClampMode mode);
protected:
    DebugPad& m_DebugPad;
    s32 m_IndexOfRead;
    bit16 m_LatestHold;
    bool m_IsReadLatestFirst;
    nn::util::SizedEnum1<StickClampMode> m_StickClampMode;
    s32 pad;
    s64 m_TickOfRead;
};

inline DebugPadReader::StickClampMode DebugPadReader::GetStickClampMode( ) const{
    return m_StickClampMode;
}

inline void DebugPadReader::SetStickClampMode(DebugPadReader::StickClampMode mode){
    m_StickClampMode = mode;
}

}
}
}
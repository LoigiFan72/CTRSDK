#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_Api.h>
#include <nn/hid/CTR/hid_TouchPanel.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/util/util_NonCopyable.h>

namespace nn{
namespace hid{
namespace CTR{

class TouchPanelReader : private nn::util::ADLFireWall::NonCopyable<TouchPanelReader>{
protected:
    TouchPanel& m_TouchPanel;
    s32 m_IndexOfRead;
    s64 m_TickOfRead;
public:
    TouchPanelReader(TouchPanel& touchPanel = CTR::GetTouchPanel( )): 
        m_TouchPanel(touchPanel), 
        m_IndexOfRead(-1), 
        m_TickOfRead(-1)
    { };
    void Read(TouchPanelStatus* pBufs, s32* pReadLen, s32 bufLen);
    bool ReadLatest(TouchPanelStatus* status);

    static const s8 MAX_READ_NUM = 7;
};

}
}
}
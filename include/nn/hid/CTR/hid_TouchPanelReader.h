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
    TouchPanel& mTouchPanel;
    s32 mIndexOfRead;
    s64 mTickOfRead;
public:
    TouchPanelReader(TouchPanel& touchPanel = CTR::GetTouchPanel( )) : mTouchPanel(touchPanel), mIndexOfRead(-1), mTickOfRead(-1){ };
    void Read(TouchPanelStatus* pBufs, s32* pReadLen, s32 bufLen);
    bool ReadLatest(TouchPanelStatus* status);

    static const s8 MAX_READ_NUM = 7;
};

}
}
}
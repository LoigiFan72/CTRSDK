// Filename: ir_CepdApi.cpp
//
// Project: Horizon

#include <nn/ir/CTR/ir_CepdApi.h>
#include <nn/hid.h>
#include <nn/hidlow.h>
#include <nn/srv.h>
#include <nn/math.h>
#include <nn/os.h>

namespace nn{
namespace ir{
namespace CTR{

nn::os::StackBuffer<1024> threadStack;

const size_t IR_WORKING_MEMORY_SIZE = 4096;
const size_t IR_WORKING_MEMORY_ALIGN = 4096;
void* irWorkingMemory = NULL;

const u8 BATTERY_UPPER_THRESHOLD = 17;
const u8 BATTERY_LOWER_THRESHOLD = 15;

nn::os::Thread cepdThread;

nn::hidlow::CTR::ExtraPadLifoRing extraPadLifoRing;
nn::hid::CTR::ExtraPadStatus oldExtraPadStatus;

nn::srv::LightEventNotificationHandler notificationConnectionHandler;
nn::srv::LightEventNotificationHandler notificationSamplingHandler;

nn::os::LightEvent samplingStartLightEvent;

const nn::fnd::TimeSpan PACKET_LOSS_CALC_TIME = nn::fnd::TimeSpan::FromMilliSeconds(1000);
const nn::fnd::TimeSpan TIMEOUT_TO_READ_DATA = nn::fnd::TimeSpan::FromMilliSeconds(20);
const nn::fnd::TimeSpan TIMEOUT_TO_WRITE_DATA = nn::fnd::TimeSpan::FromMilliSeconds(450);
const nn::fnd::TimeSpan TIMEOUT_TO_ERASE_DATA = nn::fnd::TimeSpan::FromMilliSeconds(2000);
const nn::fnd::TimeSpan TIMEOUT_TO_GET_FIRM_VERSION =  nn::fnd::TimeSpan::FromMilliSeconds(100);

const bit32 PLUSBUTTON_EMULATION_SCALE_SHIFT = 8;
const s32 PLUSBUTTON_EMULATION_SCALE  = 1 << PLUSBUTTON_EMULATION_SCALE_SHIFT;

const s16 s_Radius = nn::hid::CTR::MIN_OF_STICK_CLAMP_MODE_CIRCLE;
const s16 s_Degree = 60;
const s32 s_Tangent = static_cast<s32>(nn::math::TanDeg(s_Degree) * PLUSBUTTON_EMULATION_SCALE);

CepdStatus CepdGetStatus(){
    return cepdStatus;
}

}
}
}
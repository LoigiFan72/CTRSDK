#pragma once

#include "nn/types.h"
#include "nn/Result.h"
#include "nn/fnd/fnd_TimeSpan.h"
#include "nn/fnd/fnd_DateTime.h"

namespace nn {
namespace pl {
namespace CTR {
namespace detail{
    typedef enum _IPCPortType{
        PORT_PEDOMETER = 0,
        PORT_PEDOMETER_HID,
        PORT_PEDOMETER_SYS,
        NUM_OF_IPC_PORTS
    } IPCPortType;

}

void GetStepHistory(ushort* pStepCounts, s32 numHours, nn::fnd::DateTime start);

}
}
}
#pragma once

#include "nn/hid/CTR/hid_GyroscopeReader.h"

namespace nn{
namespace hid{
namespace CTR{
namespace detail{

class Ipc{
public:
    static Result EnableAccelerometer();
    static Result DisableAccelerometer();
    static Result EnableGyroscopeLow();
    static Result DisableGyroscopeLow();
    static Result GetGyroscopeLowCalibrateParam(GyroscopeLowCalibrateParam *);
    static Result GetGyroscopeLowRawToDpsCoefficient(f32* pCoefficient);
    static Result GetIPCHandles(Handle *pSharedMemoryHandle,Handle *pPadEvent,Handle *pTouchPanelEvent,Handle *pAccelerometerEvent,Handle *pGyroscopeLowEvent,Handle *pDebugPadEvent);

    static nn::Handle s_Session;
};

}
}
}
}
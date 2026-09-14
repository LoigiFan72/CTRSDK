#pragma once

#include <nn/types.h>

namespace nn{
namespace ir{
namespace CTR{

enum CepdStatus
{
    CEPD_STATUS_NOT_INITIALIZED = 0,
    CEPD_STATUS_NOT_CONNECTED = 1,
    CEPD_STATUS_TRY_TO_CONNECT = 2,
    CEPD_STATUS_CONNECTED = 3,
    CEPD_STATUS_SAMPLING = 4,
    CEPD_STATUS_STOP_SAMPLING = 5,
};

CepdStatus CepdGetStatus();

namespace{
    bool isStartedSampling;
    s8 peripheralId;
    s8 remainReceivableCounter;
    CepdStatus cepdStatus;
}
}
}
}
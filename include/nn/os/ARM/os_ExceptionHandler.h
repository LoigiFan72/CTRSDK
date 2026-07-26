#pragma once

#include <nn/types.h>

namespace nn{
namespace os{
namespace ARM{

struct ExceptionBuffer{
};

struct ExceptionContext{
    bit32 mResult[16];
    bit32 mCpsr;
};

struct ExceptionInfo{
    s8 mType;
    s8 unk[3];
    bit32 fsr;
    bit32 far;
    bit32 fpexc;
    bit32 fpinst;
    bit32 fpinst2;
};

enum ExceptionType{
    TYPE_PABT,
    TYPE_DABT,
    TYPE_UNDEF,
    TYPE_VFP,
    TYPE_MAX_BIT = 80,
};

typedef void (*UserExceptionHandler)(struct ExceptionInfo *, struct ExceptionContext *);

void SetUserExceptionHandler(nn::os::ARM::UserExceptionHandler pHandler, uptr stackBottom);

}
}
}
#pragma once

#include <nn/types.h>
#include <nn/util/util_SizedEnum.h>

namespace nn{
namespace os{
namespace ARM{

struct ExceptionBuffer{ };

enum ExceptionType
{
    TYPE_PABT,
    TYPE_DABT,
    TYPE_UNDEF,
    TYPE_VFP,
    TYPE_MAX_BIT = 80,
};

struct ExceptionContext
{
    bit32 r[16];
    bit32 cpsr;
};

struct ExceptionInfo
{
    util::SizedEnum1<ExceptionType> type;
    s8 pad[3];
    bit32 fsr;
    bit32 far;
    bit32 fpexc;
    bit32 fpinst;
    bit32 fpinst2;
};

typedef void (*UserExceptionHandler)(struct ExceptionInfo *, struct ExceptionContext *);

void SetUserExceptionHandler(nn::os::ARM::UserExceptionHandler pHandler, uptr stackBottom);

}
}
}
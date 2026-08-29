#pragma once

#include <nn/fnd.h>
#include <nn/cec/CTR/cec_Api.h>

namespace nn {
namespace cec {
namespace CTR {

struct CecAllocFunc
{
    void* (*allocFunc)(size_t);
    void  (*freeFunc)(void*);
};

class CecControl{
public:
    explicit  CecControl(size_t bufSize);
    CecControl();
    ~CecControl();

    static Result  Initialize(nn::fnd::IAllocator& cecAllocFunc);
    static Result  Finalize();

    static bool    IsInitialized();
    static Result  StartScanning(bool reset = false);
    static Result  Suspend();
    static Result  StopScanning(bool b_Immediate = false, bool b_Async = false);
    static cec::CTR::TitleId MakeCecTitleId(bit32 id, bit8 variation = 0x0);

private:
    static bool                s_Initialized;
    static bool                s_NdmSuspended;
    static bool                s_EnterExclusiveState;
    static nn::os::CriticalSection    m_Cs;

};

} // namespace CTR
} // namespace cec
} // namespace nn
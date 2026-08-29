#pragma once

#include <nn/os/os_Types.h>
#include <nn/fnd/fnd_TimeSpan.h>
#include <nn/fnd/fnd_InterlockedVariable.h>
#include <nn/svc.h>

namespace nn{ 
namespace os{
    class WaitableCounter{
    public:
        typedef fnd::InterlockedVariable<s32> ValueType;
        ValueType m_v;

        static nnHandle s_Handle;
    public:
        static void Initialize();
        static void Finalize(); // Unused
        ValueType&       operator* () {return m_v; }
        const ValueType& operator* () const { return m_v; }
        ValueType*       operator->() { return &m_v; }
    public:
        Result WaitIfLessThan (s32 value){
                return ArbitrateAddress(ARBITRATION_TYPE_WAIT_IF_LESS_THAN, value);
        }
        Result WaitIfLessThanWithTimeout (s32 value){
                return ArbitrateAddress(ARBITRATION_TYPE_WAIT_IF_LESS_THAN_WITH_TIMEOUT, value);
        }
        Result WaitIfLessThan(s32 value, fnd::TimeSpan timeout){
                return ArbitrateAddress(nn::os::ARBITRATION_TYPE_WAIT_IF_LESS_THAN_WITH_TIMEOUT, value, timeout);
        }
        Result DecrementAndWaitIfLessThan (s32 value){
                return ArbitrateAddress(ARBITRATION_TYPE_DECREMENT_AND_WAIT_IF_LESS_THAN, value);
        }
        Result DecrementAndWaitIfLessThanWithTimeout (s32 value){
                return ArbitrateAddress(ARBITRATION_TYPE_DECREMENT_AND_WAIT_IF_LESS_THAN_WITH_TIMEOUT, value);
        }
        Result Signal (s32 value){
                return ArbitrateAddress(ARBITRATION_TYPE_SIGNAL, value);
        }
        Result SignalAll (){
                return Signal (-1);
        }
    private:
        Result ArbitrateAddress (ArbitrationType type, s32 value){
                return svc::ArbitrateAddress (s_Handle, (uptr)&m_v, type, value, 0);
        }
        Result ArbitrateAddress(nn::os::ArbitrationType type, s32 value, fnd::TimeSpan timeout){
                return nn::svc::ArbitrateAddress(s_Handle, reinterpret_cast<uptr>(&m_v), type, value, timeout.GetNanoSeconds());
        }
    };
}
}
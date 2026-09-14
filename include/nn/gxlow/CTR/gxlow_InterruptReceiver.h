#pragma once

#include <nn/drivers/gxlow/CTR/gxlow_InterruptTable.h>
#include <nn/gxlow/CTR/detail/gxlow_DisplaySwapInfoPad.h>
#include <nn/gxlow/CTR/detail/gxlow_InterruptRelayQueue.h>

namespace nn{
namespace gxlow{
namespace CTR{
namespace detail{

class SharedWorkMem : public SharedMemoryBlock
{
public:
    void Initialize(Handle hSharedMemory)
    {
        this->AttachAndMap(hSharedMemory,0x1000,false);
    }
    uptr GetBufferForRelayQueue(s32 index)
    {
        return this->GetAddress() + index * 0x40;
    }
    uptr GetBufferForCmdReqQueue(s32 index)
    {
        return this->GetAddress() + 0x800 + index * 0x200;
    }
    uptr GetBufferForDisplaySwapInfoPad(s32 index)
    {
        return this->GetAddress() + 0x200 + index * 0x80;
    }
    };
}

class InterruptRelayQueueRx : public detail::InterruptRelayQueueBase
{
public:
    void SuppressPdcEvents(bool enable);
    Result TryDequeue(nngxlowInterrupt* pSrc);
};

class InterruptReceiver : public drivers::gxlow::CTR::InterruptTable
{
private:
    enum HandlerWaitStatus
    {
        RECEIVER_NOT_WAITING,
        RECEIVER_WAITING,
        RECEIVER_ANY_HANDLER_DONE
    };

    os::Event m_RxEvent;
    InterruptRelayQueueRx m_RelayQ;
    detail::SharedWorkMem m_SharedWorkMem;
    CmdReqQueueTx m_CmdReqQ;
    DisplaySwapInfoPadTx m_SwapInfoPad;
    os::LightEvent m_AnyHandlerDoneEvent;
    os::Thread m_ReceiverThread;
    s8 m_GspContextIndex;
    bool m_IsFirstConnection;
    util::SizedEnum1<HandlerWaitStatus> m_HandlerWaitStatus;
    bool m_FinalizeRequest;
    os::StackBuffer<0x1000> m_ThreadStack;
public:
    InterruptReceiver(){ }    

    void CallHandlerFunc(s32 index);
    void Initialize();
    void Finalize();
    CmdReqQueueTx& GetCmdReqQueue(){ return m_CmdReqQ; }
    DisplaySwapInfoPadTx& GetSwapInfoPad(){ return m_SwapInfoPad; }
    bool IsFirstConnection(){ return m_IsFirstConnection; }
    static void ReceiverThreadFunc(uptr arg);
    void SuppressPdcEvents(bool enable){ m_RelayQ.SuppressPdcEvents(enable); }
    void WaitAnyHandlerDone();
}; 

}
}
}
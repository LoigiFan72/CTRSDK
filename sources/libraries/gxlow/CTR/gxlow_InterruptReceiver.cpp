// Filename: gxlow_InterruptReceiver.cpp
//
// Project: Horizon

#include <nn/gxlow/CTR/gxlow_InterruptReceiver.h>
#include <nn/gxlow/CTR/gxlow_Result.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>

namespace nn{
namespace gxlow{
namespace CTR{

static const u32 RECEIVER_THREAD_PRIORITY = 0x5109d502;

Result InterruptRelayQueueRx::TryDequeue(nngxlowInterrupt* pSrc)
{
    Result result;
    
    if (m_pBody == NULL)
    {
        result = ResultNotInitialized();
    }
    else if (m_pBody->control.usedCount == 0)
    {
        result = ResultQueueEmpty();
    }
    else
    {
        QueueControlPacker control;

        *pSrc = m_pBody->data[m_pBody->control.head];
        
        do
        {
            control.packed32 = __ldrex(&m_pBody->control);
            
            control.qc.head = ( control.qc.head + 1 ) % QUEUE_LENGTH;
            control.qc.usedCount--;
        } while (__strex(control.packed32, &m_pBody->control) != 0);
        
        result = ResultSuccess();
        
        if (m_pBody->control.status == detail::QUEUE_ERR_FULL)
        {
            result = ResultQueueFull();
        }
    }
    
    return result;
}

void InterruptRelayQueueRx::SuppressPdcEvents(bool enable)
{
    if (m_pBody == NULL){
        return;
    }
    
    QueueControlPacker control;
    do
    {
        control.packed32 = __ldrex(&m_pBody->control);
        
        if (enable)
        {
            control.qc.control |= QUEUE_CONTROL_SUPPRESS_PDC;
        }
        else
        {
            control.qc.control &= ~QUEUE_CONTROL_SUPPRESS_PDC;
        }
        
    } while (__strex(control.packed32, &m_pBody->control) != 0);
}

/* Interrupt Rec */

void InterruptReceiver::Initialize()
{
    nn::Handle hSharedWorkMem;
    s32 index;

    this->InitializeTable();
    this->m_RxEvent.Initialize(false);
    this->m_FinalizeRequest = false;
    this->m_HandlerWaitStatus = RECEIVER_NOT_WAITING;
    this->m_AnyHandlerDoneEvent.Initialize(false);

    Gpu* gpu = detail::GetGpuIpc();
    
    bit32 attr = (detail::IsAppletMode()) ? 1 : 2;
    if (detail::IsFatalErrMode())
    {
        attr |= 4;
    }
    
    Result result = gpu->RegisterInterruptRelayQueue(this->m_RxEvent.GetHandle(),attr,&hSharedWorkMem,&index);
    m_GspContextIndex = static_cast<s8>(index);

    void* pBody;
    this->m_SharedWorkMem.Initialize(hSharedWorkMem);

    // RelayQ
    pBody = reinterpret_cast<void*>(this->m_SharedWorkMem.GetBufferForRelayQueue(index));
    this->m_RelayQ.Initialize(this->m_RxEvent.GetHandle(), pBody);
    
    // CmdReqQueue
    pBody = reinterpret_cast<void*>(this->m_SharedWorkMem.GetBufferForCmdReqQueue(index));
    this->m_CmdReqQ.Initialize(pBody);
    
    // DisplaySwapInfoPad
    pBody = reinterpret_cast<void*>(this->m_SharedWorkMem.GetBufferForDisplaySwapInfoPad(index));
    this->m_SwapInfoPad.Initialize(pBody);
    

    if (result == ResultFirstConnection())
    {
        m_IsFirstConnection = true;
    }
    else
    {
        m_IsFirstConnection = false;
    }

    this->m_ReceiverThread.Start(ReceiverThreadFunc,reinterpret_cast<uptr>(this),m_ThreadStack,RECEIVER_THREAD_PRIORITY);
    this->UnlockTable();
    
    return;
}

void InterruptReceiver::Finalize()
{
    Result result;

    m_FinalizeRequest = true;
    this->m_RxEvent.Signal();

    this->m_ReceiverThread.Join();
    this->m_ReceiverThread.Finalize();
    
    this->LockTable();

    result = detail::GetGpuIpc()->UnregisterInterruptRelayQueue();
    NN_GXLOW_RESULT_ASSERT(result, "[Finalize]");
    
    this->m_CmdReqQ.Finalize();
    this->m_RelayQ.Finalize();
    this->m_SwapInfoPad.Finalize();

    this->m_SharedWorkMem.Finalize();
    this->m_RxEvent.Finalize();
    this->FinalizeTable();
    
    return;
}

void InterruptReceiver::CallHandlerFunc(s32 index)
{
    this->LockTable();
    
    if (m_InterruptHandlerTable[index] != NULL)
    {
        m_InterruptHandlerTable[index]();
    }
    
    HandlerWaitStatus currentWaitStatus = m_HandlerWaitStatus;
    m_HandlerWaitStatus = RECEIVER_ANY_HANDLER_DONE;
    if (currentWaitStatus == RECEIVER_WAITING)
    {
        this->m_AnyHandlerDoneEvent.Signal();
    }
    
    this->UnlockTable();
}

void InterruptReceiver::WaitAnyHandlerDone()
{
    this->LockTable();
    
    if (m_HandlerWaitStatus != RECEIVER_ANY_HANDLER_DONE)
    {
        m_HandlerWaitStatus = RECEIVER_WAITING;
        this->UnlockTable();
        
        this->m_AnyHandlerDoneEvent.Wait();
    }
    else
    {
        m_HandlerWaitStatus = RECEIVER_NOT_WAITING;
        this->UnlockTable();
    }
}

void InterruptReceiver::ReceiverThreadFunc(uptr arg)
{
    InterruptReceiver* pThis = reinterpret_cast<InterruptReceiver*>(arg);

    for(;;)
    {
        nn::Result result;

        pThis->m_RxEvent.Wait();
        
        pThis->m_RxEvent.ClearSignal();
        
        if (pThis->m_FinalizeRequest )
            break;
        
        for(;;)
        {
            nngxlowInterrupt src;
            result = pThis->m_RelayQ.TryDequeue(&src);
            if (result == ResultQueueEmpty())
                break;

            pThis->CallHandlerFunc(src);
        }
    }
}

}
}
}
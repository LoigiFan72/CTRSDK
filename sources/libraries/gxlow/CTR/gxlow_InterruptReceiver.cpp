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

Result InterruptRelayQueueRx::TryDequeue(nngxlowInterrupt* pSrc){
    Result result;
    
    if (mpBody == NULL){
        result = ResultNotInitialized();
    }
    else if (mpBody->control.usedCount == 0 ){
        result = ResultQueueEmpty();
    }
    else{
        QueueControlPacker control;

        *pSrc = mpBody->data[mpBody->control.head];
        
        do{
            control.packed32 = __ldrex(&mpBody->control);
            
            control.qc.head = ( control.qc.head + 1 ) % QUEUE_LENGTH;
            control.qc.usedCount--;
        } while (__strex(control.packed32, &mpBody->control) != 0);
        
        result = ResultSuccess();
        
        if (mpBody->control.status == detail::QUEUE_ERR_FULL){
            result = ResultQueueFull();
        }
    }
    
    return result;
}

void InterruptRelayQueueRx::SuppressPdcEvents(bool enable){
    if (mpBody == NULL){
        return;
    }
    
    QueueControlPacker control;
    do{
        control.packed32 = __ldrex(&mpBody->control);
        
        if (enable){
            control.qc.control |= QUEUE_CONTROL_SUPPRESS_PDC;
        }
        else{
            control.qc.control &= ~QUEUE_CONTROL_SUPPRESS_PDC;
        }
        
    } while (__strex(control.packed32, &mpBody->control) != 0);
}

/* Interrupt Rec */

void InterruptReceiver::Initialize(void){
    nn::Handle hSharedWorkMem;
    s32 index;

    this->InitializeTable();
    this->mRxEvent.Initialize(false);
    this->mFinalizeRequest = false;
    this->mHandlerWaitStatus = RECEIVER_NOT_WAITING;
    this->mAnyHandlerDoneEvent.Initialize(false);

    Gpu* gpu = detail::GetGpuIpc();
    
    bit32 attr = (detail::IsAppletMode() ) ? 1 : 2;
    if (detail::IsFatalErrMode()){
        attr |= 4;
    }
    
    Result result = gpu->RegisterInterruptRelayQueue(this->mRxEvent.GetHandle(),attr,&hSharedWorkMem,&index);
    mGspContextIndex = static_cast<s8>(index);

    void* pBody;
    this->mSharedWorkMem.Initialize(hSharedWorkMem);

    // RelayQ
    pBody = reinterpret_cast<void*>(this->mSharedWorkMem.GetBufferForRelayQueue(index));
    this->mRelayQ.Initialize(this->mRxEvent.GetHandle(), pBody);
    
    // CmdReqQueue
    pBody = reinterpret_cast<void*>(this->mSharedWorkMem.GetBufferForCmdReqQueue(index));
    this->mCmdReqQ.Initialize(pBody);
    
    // DisplaySwapInfoPad
    pBody = reinterpret_cast<void*>(this->mSharedWorkMem.GetBufferForDisplaySwapInfoPad(index));
    this->mSwapInfoPad.Initialize(pBody);
    

    if (result == ResultFirstConnection()){
        mIsFirstConnection = true;
    }
    else{
        mIsFirstConnection = false;
    }

    this->mReceiverThread.Start(ReceiverThreadFunc,reinterpret_cast<uptr>(this),mThreadStack,RECEIVER_THREAD_PRIORITY);
    this->UnlockTable();
    
    return;
}

void InterruptReceiver::Finalize(void){
    Result result;

    mFinalizeRequest = true;
    this->mRxEvent.Signal();

    this->mReceiverThread.Join();
    this->mReceiverThread.Finalize();
    
    this->LockTable();

    result = detail::GetGpuIpc()->UnregisterInterruptRelayQueue();
    NN_GXLOW_RESULT_ASSERT(result, "[Finalize]");
    
    this->mCmdReqQ.Finalize();
    this->mRelayQ.Finalize();
    this->mSwapInfoPad.Finalize();

    this->mSharedWorkMem.Finalize();
    this->mRxEvent.Finalize();
    this->FinalizeTable();
    
    return;
}

void InterruptReceiver::CallHandlerFunc(s32 index){
    this->LockTable();
    
    if (mInterruptHandlerTable[index] != NULL){
        mInterruptHandlerTable[index]();
    }
    
    HandlerWaitStatus currentWaitStatus = mHandlerWaitStatus;
    mHandlerWaitStatus = RECEIVER_ANY_HANDLER_DONE;
    if (currentWaitStatus == RECEIVER_WAITING){
        this->mAnyHandlerDoneEvent.Signal();
    }
    
    this->UnlockTable();
}

void InterruptReceiver::WaitAnyHandlerDone( void ){
    this->LockTable();
    
    if (mHandlerWaitStatus != RECEIVER_ANY_HANDLER_DONE){
        mHandlerWaitStatus = RECEIVER_WAITING;
        this->UnlockTable();
        
        this->mAnyHandlerDoneEvent.Wait();

    }
    else{
        mHandlerWaitStatus = RECEIVER_NOT_WAITING;
        this->UnlockTable();
    }
}

void InterruptReceiver::ReceiverThreadFunc(uptr arg){
    InterruptReceiver* pThis = reinterpret_cast<InterruptReceiver*>(arg);

    for(;;){
        nn::Result result;

        pThis->mRxEvent.Wait();
        
        pThis->mRxEvent.ClearSignal();
        
        if (pThis->mFinalizeRequest )
            break;
        
        for(;;){
            nngxlowInterrupt src;
            result = pThis->mRelayQ.TryDequeue(&src);
            if (result == ResultQueueEmpty())
                break;

            pThis->CallHandlerFunc(src);
        }
    }
}

}
}
}
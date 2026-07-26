#pragma once

#include <nn/drivers/gxlow/CTR/gxlow_InterruptTable.h>

namespace nn{
namespace gxlow{
namespace CTR{
namespace detail{
    class SharedWorkMem : public SharedMemoryBlock{
    public:
        void Initialize(Handle hSharedMemory){
            this->AttachAndMap(hSharedMemory,0x1000,false);
        }
        uptr GetBufferForRelayQueue(s32 index){
            return this->GetAddress() + index * 0x40;
        }
        uptr GetBufferForCmdReqQueue(s32 index){
            return this->GetAddress() + 0x800 + index * 0x200;
        }
        uptr GetBufferForDisplaySwapInfoPad(s32 index){
            return this->GetAddress() + 0x200 + index * 0x80;
        }
    };
}
    class InterruptRelayQueueBase{
    protected:
        os::Event mRxEvent;
        CmdReqQueueBase::QueueBody* mpBody;
    public:
        void Initialize(Handle eventHandle, void* pQueueBody){
            this->mRxEvent.SetHandle(eventHandle);
            NN_TASSERT_(pQueueBody);
            this->mpBody = (CmdReqQueueBase::QueueBody*)pQueueBody;
        }
        void Finalize(){
            this->mRxEvent.ClearHandle();
            this->mpBody = 0;
        }
    };

    class InterruptRelayQueueRx : public InterruptRelayQueueBase{
    public:
        void SuppressPdcEvents(bool enable);

        Result TryDequeue(nngxlowInterrupt* pSrc){
            Result res;
            // TODO
        }
    };

    class InterruptReceiver : public drivers::gxlow::CTR::InterruptTable{
    private:
        enum HandlerWaitStatus{
            RECEIVER_NOT_WAITING,
            RECEIVER_WAITING,
            RECEIVER_ANY_HANDLER_DONE
        };
        os::Event mRxEvent;
        InterruptRelayQueueRx mRelayQ;
        detail::SharedWorkMem mSharedWorkMem;
        CmdReqQueueTx mCmdReqQ;
        DisplaySwapInfoTx mSwapInfoPad;
        os::LightEvent mAnyHandlerDoneEvent;
        os::Thread mReceiverThread;
        s8 mGspContextIndex;
        bool mIsFirstConnection;
        util::SizedEnum1<HandlerWaitStatus> mHandlerWaitStatus;
        bool mFinalizeRequest;
        os::StackBuffer<0x1000> mThreadStack;
    public:
        InterruptReceiver(){ }    

        void CallHandlerFunc(s32 index){
            this->LockTable();
            if(this->mInterruptHandlerTable[index])
                this->mInterruptHandlerTable[index]();
            HandlerWaitStatus status = this->mHandlerWaitStatus;
            memcpy(&this->mHandlerWaitStatus,&status,1);
            if(status == RECEIVER_WAITING)
                this->mAnyHandlerDoneEvent.Signal();
            this->UnlockTable();
        }
        void Initialize();
        void Finalize();
        CmdReqQueueTx* GetCmdReqQueue(){ return &this->mCmdReqQ; }
        DisplaySwapInfoTx* GetSwapInfoPad(){ return &this->mSwapInfoPad; }
        bool IsFirstConnection(){ return this->mIsFirstConnection; }
        static void ReceiverThreadFunc(InterruptReceiver* arg);
        void SuppressPdcEvents(bool enable){ this->mRelayQ.SuppressPdcEvents(enable); }
        void WaitAnyHandlerDone();
    };  
}
}
}
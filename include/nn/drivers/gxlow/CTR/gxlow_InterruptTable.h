#pragma once

#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/gxlow/CTR/detail/gxlow_CmdReqQueue.h>
#include <nn/os.h>
#include <nn/os/os_Event.h>

namespace nn{
namespace drivers{
namespace gxlow{
namespace CTR{

class InterruptTable{
public:
    os::CriticalSection mHandlerLock;
    int pad;
    nngxlowFuncPtr mInterruptHandlerTable[NN_GXLOW_NUM_INTERRUPTS];
public:
    InterruptTable(){ }
    void LockTable(){
        this->mHandlerLock.Enter();
    }
    void UnlockTable(){
        this->mHandlerLock.Leave();
    }

    nngxlowFuncPtr RegisterInterruptHandler(nngxlowFuncPtr interruptHandler, nngxlowInterrupt interruptType){
        nngxlowFuncPtr gxptr;
        uint type = interruptType;
        if(type < NN_GXLOW_NUM_INTERRUPTS){
            this->LockTable();
            gxptr = mInterruptHandlerTable[type];
            mInterruptHandlerTable[type] = interruptHandler;
            this->UnlockTable();
        }
        else{
            NN_TASSERTMSG_(!gxptr, "Invalid interrupt type %d.\n", type);
        }
        return gxptr;
    }

    void InitializeTable(){
        this->mHandlerLock.Initialize();
        this->LockTable();
        for(int i = 0; i < NN_GXLOW_NUM_INTERRUPTS; i++){
            mInterruptHandlerTable[i] = 0;
        }
    }

    void FinalizeTable(){
        for(int i = 0; i < NN_GXLOW_NUM_INTERRUPTS; i++){
            mInterruptHandlerTable[i] = 0;
        }
        this->UnlockTable();
        this->mHandlerLock.Finalize();
    }
};
}
}
}
}
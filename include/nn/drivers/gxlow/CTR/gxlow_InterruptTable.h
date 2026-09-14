#pragma once

#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/gxlow/CTR/detail/gxlow_CmdReqQueue.h>
#include <nn/os.h>
#include <nn/os/os_Event.h>

namespace nn{
namespace drivers{
namespace gxlow{
namespace CTR{

class InterruptTable
{
public:
    os::CriticalSection m_HandlerLock;
    int pad;
    nngxlowFuncPtr m_InterruptHandlerTable[NN_GXLOW_NUM_INTERRUPTS];
public:
    InterruptTable()
    { 
    }

    void LockTable()
    {
        this->m_HandlerLock.Enter();
    }
    void UnlockTable()
    {
        this->m_HandlerLock.Leave();
    }

    nngxlowFuncPtr RegisterInterruptHandler(nngxlowFuncPtr interruptHandler, nngxlowInterrupt interruptType)
    {
        nngxlowFuncPtr gxptr;
        uint type = interruptType;
        if(type < NN_GXLOW_NUM_INTERRUPTS)
        {
            this->LockTable();
            gxptr = m_InterruptHandlerTable[type];
            m_InterruptHandlerTable[type] = interruptHandler;
            this->UnlockTable();
        }
        else
        {
            NN_TASSERTMSG_(!gxptr, "Invalid interrupt type %d.\n", type);
        }
        return gxptr;
    }

    void InitializeTable()
    {
        this->m_HandlerLock.Initialize();
        this->LockTable();
        for(int i = 0; i < NN_GXLOW_NUM_INTERRUPTS; i++)
        {
            m_InterruptHandlerTable[i] = 0;
        }
    }

    void FinalizeTable()
    {
        for(int i = 0; i < NN_GXLOW_NUM_INTERRUPTS; i++)
        {
            m_InterruptHandlerTable[i] = 0;
        }
        this->UnlockTable();
        this->m_HandlerLock.Finalize();
    }
};
}
}
}
}
// Filename: gxlow_CmdReqQueue.cpp
//
// Project: Horizon

#include <nn/os/ARM/os_MemoryBarrier.h>
#include <nn/gxlow/CTR/gxlow_RegAccess.h>
#include <nn/gxlow/CTR/gxlow_Result.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>
#include <nn/gxlow/CTR/detail/gxlow_CmdReqQueue.h>

namespace nn {
namespace gxlow {
namespace CTR {

void CmdReqQueueTx::Initialize(void* pQueueBody)
{
    detail::CmdReqQueueBase::Initialize(pQueueBody);
    this->Reset();
}

void CmdReqQueueTx::Finalize()
{
    this->Reset();
    detail::CmdReqQueueBase::Finalize();
}

Result CmdReqQueueTx::TryEnqueue(const detail::CmdReq* pCmdReq)
{
    if (m_pBody == NULL)
        return ResultNotInitialized();
    
    QueueControlPacker control;

    if (QUEUE_LENGTH > m_pBody->control.usedCount)
    {
        control.packed32 = __ldrex(&m_pBody->control);
        
        s32 lastIndex = (control.qc.head + control.qc.usedCount) % QUEUE_LENGTH;
        mpBody->data[lastIndex] = *pCmdReq;
        
        nn::os::ARM::DataSynchronizationBarrier();
        
        control.qc.usedCount++;
        while (__strex(control.packed32, &m_pBody->control) != 0)
        {
            control.packed32 = __ldrex(&m_pBody->control);
            control.qc.usedCount++;
        }
        
        if (control.qc.usedCount == 1)
        {
            detail::GetGpuIpc()->TriggerCmdReqQueue();
        }
    }
    else
    {
        return ResultQueueFull();
    }
    
    return ResultSuccess();
}

void CmdReqQueueTx::Reset()
{
    QueueControlPacker control;
    
    do
    {
        control.packed32 = __ldrex(&m_pBody->control);
        
        if (control.qc.head >= QUEUE_LENGTH)
        {
            control.qc.head = 0;
        }
        control.qc.usedCount = 0;
        control.qc.status    = 0;
        control.qc.control   = 0;
    } while (__strex(control.packed32, &m_pBody->control) != 0);
}

}
}
}
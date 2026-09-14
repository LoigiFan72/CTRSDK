#pragma once

#include <nn/gxlow/CTR/gxlow_CTR.h>
#include <nn/os.h>
#include <nn/util.h>

namespace nn {
namespace gxlow {
namespace CTR {
namespace detail {

enum QueueStatus
{
    QUEUE_STATUS_OK = 0,
    QUEUE_ERR_FULL  = 1
};
    
class InterruptRelayQueueBase
{
public:
    static const size_t QUEUE_BODY_SIZE = 64;
        
    InterruptRelayQueueBase(): 
        m_pBody(0) 
    {
    }

    ~InterruptRelayQueueBase() 
    {
    }
        
    void Initialize(nn::Handle eventHandle,void* pQueueBody);
    void Finalize();
    
protected:
    static const s32  QUEUE_LENGTH = 52;
    static const s32  QUEUE_HIGH_WATERMARK = 32;
    static const bit8 QUEUE_CONTROL_SUPPRESS_PDC = 0x01;
        
    struct QueueControl
    {
        u8 head;
        u8 usedCount;
        bit8 status;
        bit8 control;
    };
        
    union QueueControlPacker
    {
        QueueControl qc;
        bit32 packed32;
    };
        
    struct QueueBody
    {
        QueueControl control;
        s32 droppedPdc0Count;
        s32 droppedPdc1Count;
        nn::util::SizedEnum1<nngxlowInterrupt> data[QUEUE_LENGTH];
    };
        
    nn::os::Event   m_RxEvent;
    QueueBody*      m_pBody;
};

inline void InterruptRelayQueueBase::Initialize(nn::Handle  eventHandle,void* pQueueBody)
{
    this->m_RxEvent.SetHandle(eventHandle);
    
    NN_TASSERT_(pQueueBody != 0);
    m_pBody = reinterpret_cast<QueueBody*>(pQueueBody);
}

inline void InterruptRelayQueueBase::Finalize()
{
    this->m_RxEvent.DetachHandle();
    m_pBody = NULL;
}


}
}
}
}
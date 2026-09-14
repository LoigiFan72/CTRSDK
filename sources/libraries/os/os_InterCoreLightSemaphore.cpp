// Filename: os_InterCoreLightSemaphore.cpp
//
// Project: Horizon

#include <nn/os/os_InterCoreLightSemaphore.h>

namespace nn{
namespace os{

s32 InterCoreLightSemaphore::Release(s32 releaseCount /*= 1*/)
{
    NN_MIN_TASSERT_(releaseCount, 1);

    LimitedAdd updater;
    updater.max   = m_Max;
    updater.value = releaseCount;

    this->m_Counter->AtomicUpdateConditional(updater);
    const s32 beforeUpdate = updater.beforeUpdate;

    ARM::DataSynchronizationBarrier();
    if((beforeUpdate <= 0) || (m_NumWaiting > 0))
    {
        this->m_Counter.Signal(releaseCount);
    }

    return beforeUpdate;
}

}
}
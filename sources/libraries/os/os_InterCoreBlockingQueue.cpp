// Filename: os_InterCoreBlockingQueue.cpp
//
// Project: Horizon

#include <nn/assert.h>
#include <nn/os/os_InterCoreBlockingQueue.h>
#include <nn/os/os_Mutex.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/fnd/fnd_InterlockedVariable.h>

using namespace nn;
using namespace nn::fnd;
using namespace nn::svc;
using namespace nn::os;
using namespace nn::util;

namespace nn{ 
namespace os{
namespace detail{

#if NN_VERSION_MAJOR <= 2

template <class Locker>
InterCoreBlockingQueueBase<Locker>::~InterCoreBlockingQueueBase()
{
    Finalize();
}

template <class Locker>
void InterCoreBlockingQueueBase<Locker>::Initialize(uptr buffer[], size_t size)
{
    m_ppBuffer      = buffer;
    m_Size          = size;
    m_FirstIndex    = 0;
    m_UsedCount     = 0;
    m_WaitingEnqueueCount = 0;
    m_WaitingDequeueCount = 0;
    m_EnqueueSemaphore.Initialize(0);
    m_DequeueSemaphore.Initialize(0);
    m_cs.Initialize();
}

template <class Locker>
Result InterCoreBlockingQueueBase<Locker>::TryInitialize(uptr buffer[], size_t size)
{
    m_ppBuffer      = buffer;
    m_Size          = size;
    m_FirstIndex    = 0;
    m_UsedCount     = 0;
    m_WaitingEnqueueCount = 0;
    m_WaitingDequeueCount = 0;

    m_EnqueueSemaphore.Initialize(0);
    m_DequeueSemaphore.Initialize(0);

    NN_UTIL_RETURN_IF_FAILED(m_cs.TryInitialize());
    return ResultSuccess();
}

template <class Locker>
void InterCoreBlockingQueueBase<Locker>::Finalize()
{
    m_cs.Finalize();
    m_DequeueSemaphore.Finalize();
    m_EnqueueSemaphore.Finalize();
}

template <class Locker>
inline void InterCoreBlockingQueueBase<Locker>::NotifyEnqueue() const
{
    if (m_WaitingEnqueueCount > 0)
    {
        m_EnqueueSemaphore.Release();
    }
}

template <class Locker>
inline void BlockingQueueBase<Locker>::NotifyDequeue() const
{
    if (m_WaitingDequeueCount > 0)
    {
        m_DequeueSemaphore.Release();
    }
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryEnqueue(uptr data)
{
    ScopedLock locker(m_cs);

    if (m_Size > m_UsedCount)
    {
        s32 lastIndex = (m_FirstIndex + m_UsedCount) % m_Size;
        m_ppBuffer[lastIndex] = data;
        m_UsedCount++;

        NotifyEnqueue();
        return true;
    }
    else
    {
        return false;
    }
}

template <class Locker>
bool BlockingQueueBase<Locker>::ForceEnqueue(uptr data, uptr* pOut)
{
    ScopedLock locker(m_cs);
    bool bReturn;
    s32 lastIndex = (m_FirstIndex + m_UsedCount) % m_Size;
    if (m_Size > m_UsedCount)
    {
        m_UsedCount++;
        bReturn = true;
    }
    else
    {
        if (pOut){
            *pOut = m_ppBuffer[lastIndex];
        }
        m_FirstIndex = (m_FirstIndex + 1) % m_Size;
        bReturn = false;
    }

    m_ppBuffer[lastIndex] = data;

    NotifyEnqueue();
    return bReturn;
}

template <class Locker>
void BlockingQueueBase<Locker>::Enqueue(uptr data)
{
    ++m_WaitingDequeueCount;
    ARM::DataSynchronizationBarrier();
    for(;;)
    {
        if (TryEnqueue(data))
        {
            break;
        }

        m_DequeueSemaphore.Acquire();
    }
    --m_WaitingDequeueCount;
    ARM::DataSynchronizationBarrier();
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryJam(uptr data)
{
    ScopedLock locker(m_cs);

    if (m_Size > m_UsedCount)
    {
        m_FirstIndex = (m_FirstIndex + m_Size - 1) % m_Size;
        m_ppBuffer[m_FirstIndex] = data;
        m_UsedCount++;

        NotifyEnqueue();
        return true;
    }
    else
    {
        return false;
    }
}

template <class Locker>
void BlockingQueueBase<Locker>::Jam(uptr data)
{
    ++m_WaitingDequeueCount;
    ARM::DataSynchronizationBarrier();
    for(;;)
    {
        if (TryJam(data))
        {
            break;
        }

        m_DequeueSemaphore.Acquire();
    }
    --m_WaitingDequeueCount;
    ARM::DataSynchronizationBarrier();
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryDequeue(uptr* pOut)
{
    ScopedLock locker(m_cs);

    if (0 < m_UsedCount)
    {
        *pOut = m_ppBuffer[m_FirstIndex];
        m_FirstIndex = (m_FirstIndex + 1) % m_Size;
        m_UsedCount--;

        NotifyDequeue();
        return true;
    }
    else
    {
        return false;
    }
}

template <class Locker>
uptr BlockingQueueBase<Locker>::Dequeue()
{
    ++m_WaitingEnqueueCount;
    ARM::DataSynchronizationBarrier();
    uptr data;
    for(;;)
    {
        if (TryDequeue(&data))
        {
            break;
        }

        mEnqueueSemaphore.Acquire();
    }
    --m_WaitingEnqueueCount;
    ARM::DataSynchronizationBarrier();
    return data;
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryGetFront(uptr* pOut) const
{
    ScopedLock locker(m_cs);

    if (0 < m_UsedCount)
    {
        *pOut = m_ppBuffer[m_FirstIndex];

        return true;
    }
    else
    {
        return false;
    }
}

template <class Locker>
uptr BlockingQueueBase<Locker>::GetFront() const
{
    ++m_WaitingEnqueueCount;
    ARM::DataSynchronizationBarrier();
    uptr data;
    for(;;)
    {
        if (TryGetFront(&data))
        {
            break;
        }

        m_EnqueueSemaphore.Acquire();
    }
    --m_WaitingEnqueueCount;
    ARM::DataSynchronizationBarrier();
    return data;
}

template class InterCoreBlockingQueueBase<nn::os::InterCoreCriticalSection>;

#endif

} // namespace detail
} // namespace os
} // namespace nn
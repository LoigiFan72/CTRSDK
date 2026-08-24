// Filename: os_BlockingQueue.cpp
//
// Project: Horizon

#include <nn/assert.h>
#include <nn/os/os_BlockingQueue.h>
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

template <class Locker>
BlockingQueueBase<Locker>::~BlockingQueueBase()
{
    Finalize();
}

template <class Locker>
void BlockingQueueBase<Locker>::Initialize(uptr buffer[], size_t size)
{
    mppBuffer      = buffer;
    mSize          = size;
    mFirstIndex    = 0;
    mUsedCount     = 0;
    mWaitingEnqueueCount = 0;
    mWaitingDequeueCount = 0;
    mEnqueueSemaphore.Initialize(0);
    mDequeueSemaphore.Initialize(0);
    mCs.Initialize();
}

template <class Locker>
Result BlockingQueueBase<Locker>::TryInitialize(uptr buffer[], size_t size)
{
    mppBuffer      = buffer;
    mSize          = size;
    mFirstIndex    = 0;
    mUsedCount     = 0;
    mWaitingEnqueueCount = 0;
    mWaitingDequeueCount = 0;

    mEnqueueSemaphore.Initialize(0);
    mDequeueSemaphore.Initialize(0);

    NN_UTIL_RETURN_IF_FAILED(mCs.TryInitialize());
    return ResultSuccess();
}

template <class Locker>
void BlockingQueueBase<Locker>::Finalize()
{
    mCs.Finalize();
    mDequeueSemaphore.Finalize();
    mEnqueueSemaphore.Finalize();
}

template <class Locker>
inline void BlockingQueueBase<Locker>::NotifyEnqueue() const
{
    if (mWaitingEnqueueCount > 0){
        mEnqueueSemaphore.Release();
    }
}

template <class Locker>
inline void BlockingQueueBase<Locker>::NotifyDequeue() const
{
    if (mWaitingDequeueCount > 0){
        mDequeueSemaphore.Release();
    }
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryEnqueue(uptr data)
{
    ScopedLock locker(mCs);

    if (mSize > mUsedCount){
        s32 lastIndex = (mFirstIndex + mUsedCount) % mSize;
        mppBuffer[lastIndex] = data;
        musedCount++;

        NotifyEnqueue();
        return true;
    }
    else{
        return false;
    }
}

template <class Locker>
bool BlockingQueueBase<Locker>::ForceEnqueue(uptr data, uptr* pOut)
{
    ScopedLock locker(mCs);
    bool bReturn;
    s32 lastIndex = (mFirstIndex + mUsedCount) % mSize;
    if (mSize > mUsedCount){
        mUsedCount++;
        bReturn = true;
    }
    else{
        if (pOut){
            *pOut = mppBuffer[lastIndex];
        }
        mFirstIndex = (mFirstIndex + 1) % mSize;
        bReturn = false;
    }

    mppBuffer[lastIndex] = data;

    NotifyEnqueue();
    return bReturn;
}

template <class Locker>
void BlockingQueueBase<Locker>::Enqueue(uptr data)
{
    ++mWaitingDequeueCount;
    for(;;){
        if (TryEnqueue(data)){
            break;
        }

        mDequeueSemaphore.Acquire();
    }
    --mWaitingDequeueCount;
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryJam(uptr data)
{
    ScopedLock locker(mCs);

    if (mSize > mUsedCount){
        mFirstIndex = (mFirstIndex + mSize - 1) % mSize;
        mppBuffer[mFirstIndex] = data;
        mUsedCount++;

        NotifyEnqueue();
        return true;
    }
    else{
        return false;
    }
}

template <class Locker>
void BlockingQueueBase<Locker>::Jam(uptr data)
{
    ++mWaitingDequeueCount;
    for(;;){
        if (TryJam(data)){
            break;
        }

        mDequeueSemaphore.Acquire();
    }
    --mWaitingDequeueCount;
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryDequeue(uptr* pOut)
{
    ScopedLock locker(mCs);

    if (0 < m_usedCount){
        *pOut = mppBuffer[mFirstIndex];
        mFirstIndex = (mFirstIndex + 1) % mSize;
        mUsedCount--;

        NotifyDequeue();
        return true;
    }
    else{
        return false;
    }
}

template <class Locker>
uptr BlockingQueueBase<Locker>::Dequeue()
{
    ++mWaitingEnqueueCount;
    uptr data;
    for(;;){
        if (TryDequeue(&data)){
            break;
        }

        mEnqueueSemaphore.Acquire();
    }
    --mWaitingEnqueueCount;
    return data;
}

template <class Locker>
bool BlockingQueueBase<Locker>::TryGetFront(uptr* pOut) const
{
    ScopedLock locker(mCs);

    if (0 < mUsedCount){
        *pOut = mppBuffer[mFirstIndex];

        return true;
    }
    else{
        return false;
    }
}

template <class Locker>
uptr BlockingQueueBase<Locker>::GetFront() const
{
    ++mWaitingEnqueueCount;
    uptr data;
    for(;;){
        if (TryGetFront(&data)){
            break;
        }

        mEnqueueSemaphore.Acquire();
    }
    --mWaitingEnqueueCount;
    return data;
}

template class BlockingQueueBase<nn::os::CriticalSection>;

} // namespace detail
} // namespace os
} // namespace nn
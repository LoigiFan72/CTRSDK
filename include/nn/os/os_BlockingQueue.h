#pragma once

#include <nn/os/os_LightSemaphore.h>
#include <nn/os/os_Mutex.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/fnd/fnd_InterlockedVariable.h>
#include <nn/util/util_NonCopyable.h>


namespace nn{ 
namespace os{
namespace detail{

template <class Locker>
class BlockingQueueBase : private nn::util::ADLFireWall::NonCopyable<BlockingQueueBase<Locker> >{
protected:
    BlockingQueueBase() {}
    BlockingQueueBase(uptr buffer[], size_t size) { Initialize(buffer, size); }
    ~BlockingQueueBase();
    void Initialize(uptr buffer[], size_t size);
    nn::Result TryInitialize(uptr buffer[], size_t size);
    void Finalize();
    void Enqueue(uptr data);
    bool TryEnqueue(uptr data);
    bool ForceEnqueue(uptr data, uptr* pOut);
    void Jam(uptr data);
    bool TryJam(uptr data);
    uptr Dequeue();
    bool TryDequeue(uptr* pOut);
    uptr GetFront() const;
    bool TryGetFront(uptr* pOut) const;
    
    s32 GetWaitingEnqueueCount() const{ return mWaitingEnqueueCount; }
    s32 GetWaitingDequeueCount() const{ return mWaitingDequeueCount; }
    s32 GetSize() const{ return mSize; }
    s32 GetUsedCount() const{ return mUsedCount; }
    s32 GetFirstIndex() const{ return mFirstIndex; }

private:
    typedef typename Locker::ScopedLock ScopedLock;
    
    uptr*                   mppBuffer;
    mutable LightSemaphore  mEnqueueSemaphore;
    mutable LightSemaphore  mDequeueSemaphore;
    mutable Locker          mCs;
    size_t                  mSize;
    s32                     mFirstIndex;
    s32                     mUsedCount;
    mutable nn::fnd::InterlockedVariable<s32> mWaitingEnqueueCount;
    mutable nn::fnd::InterlockedVariable<s32> mWaitingDequeueCount;

    void NotifyEnqueue() const;
    void NotifyDequeue() const;
};

} // namespace detail

class BlockingQueue : private os::detail::BlockingQueueBase<nn::os::CriticalSection>{
private:
    typedef os::detail::BlockingQueueBase<nn::os::CriticalSection> Base;
public:
    BlockingQueue() {}
    BlockingQueue(uptr buffer[], size_t size): 
        Base(buffer, size) 
    {}
    ~BlockingQueue() { this->Finalize(); }

    void Initialize(uptr buffer[], size_t size) { Base::Initialize(buffer, size); }
    Result TryInitialize(uptr buffer[], size_t size) { return Base::TryInitialize(buffer, size); }

    void Finalize() { Base::Finalize(); }

    void Enqueue(uptr data) { Base::Enqueue(data); }
    bool TryEnqueue(uptr data) { return Base::TryEnqueue(data); }

    void Jam(uptr data) { Base::Jam(data); }
    bool TryJam(uptr data) { return Base::TryJam(data); }

    uptr Dequeue() { return Base::Dequeue(); }
    bool TryDequeue(uptr* pOut) { return Base::TryDequeue(pOut); }

    uptr GetFront() const { return Base::GetFront(); }
    bool TryGetFront(uptr* pOut) const { return Base::TryGetFront(pOut); }

    using Base::GetSize;
    using Base::GetUsedCount;
    using Base::GetFirstIndex;
};

} // namespace os
} // namespace nn
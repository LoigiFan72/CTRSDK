#pragma once

#include "nn/types.h"
#include "nn/fnd/fnd_Allocator.h"
#include "nn/fnd/fnd_HeapBase.h"
#include "nn/os/os_CriticalSection.h"
#include "nn/os/os_LockPolicy.h"
#include "nn/Assert.h"

// 100%

#define NN_FND_FRAMEHEAP_FREE_HEAD  1
#define NN_FND_FRAMEHEAP_FREE_TAIL  2
#define NN_FND_FRAMEHEAP_FREE_ALL   (NN_FND_FRAMEHEAP_FREE_HEAD | NN_FND_FRAMEHEAP_FREE_TAIL)

#define NN_FND_FRAMEHEAP_ADJUST_TAIL  1
#define NN_FND_FRAMEHEAP_ADJUST_HEAD -1

namespace nn{ 
namespace fnd{

class FrameHeapBase : public HeapBase {
public:
    class State;
protected:
    FrameHeapBase() : mAddr(0){}
    virtual ~FrameHeapBase(){ this->Finalize(); }
    virtual void FreeV(void* p){}
    virtual void* GetStartAddress() const{ return reinterpret_cast<void*>(mAddr); }
    virtual size_t GetTotalSize() const{ return mSize; }
    virtual void Dump() const;
    virtual bool HasAddress(const void* addr) const{ 
        return mAddr <= reinterpret_cast<uptr>(addr) && reinterpret_cast<uptr>(addr) < (mAddr + mSize);
    }

    void Finalize() { mAddr = 0; }

private:
    uptr   mAddr;
    size_t mSize;
    uptr   mCurrentHead;
    uptr   mCurrentTail;
};

class FrameHeapBase::State {
public:
    explicit State(FrameHeapBase& heap) : mHead(heap.mCurrentHead), mTail(heap.mCurrentTail) {}
    State() : mHead(0), mTail(0) {}

private:
    uptr mHead;
    uptr mTail;

    explicit State(uptr head, uptr tail) : mHead(head), mTail(tail) {}

    friend class FrameHeapBase;
};

template <class LockPolicy>
class FrameHeapTemplate : public FrameHeapBase, private LockPolicy::LockObject {
private:
    typedef FrameHeapBase Base;
    typedef typename LockPolicy::LockObject LockObject;
    typedef typename LockPolicy::ScopedLock ScopedLock;

public:
    FrameHeapTemplate() {}

    static FrameHeapTemplate* Create(HeapBase* parent, void* addr, size_t size, bit32 option = 0, bit32 placement = HEAP_INFOPLACEMENT_HEAD);

    virtual ~FrameHeapTemplate() {}
    virtual void FreeV(void*) { NN_TASSERT_(0); }
    virtual void* GetStartAddress() const {
        ScopedLock lk(*this);
        return Base::GetStartAddress();
    }

    virtual size_t GetTotalSize() const {
        ScopedLock lk(*this);
        return Base::GetTotalSize();
    }

    virtual bool HasAddress(const void* addr) const {
        ScopedLock lk(*this);
        return Base::HasAddress(addr);
    }

    virtual void Dump() const {
        Base::Dump();
    }
};

}
}
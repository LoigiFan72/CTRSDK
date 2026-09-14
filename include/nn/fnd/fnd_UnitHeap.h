#pragma once

#include <nn/types.h>
#include <nn/fnd/fnd_HeapBase.h>
#include <nn/fnd/fnd_Allocator.h>
#include <nn/os/os_LockPolicy.h>
#include <nn/os/os_CriticalSection.h>

// Things that are needed are done here.

namespace nn {
namespace fnd {
class UnitHeapBase : public HeapBase 
{
public:
    static size_t GetRequiredHeapSize(size_t unit, size_t numUnit, s32 alignment = DEFAULT_ALIGNMENT);
    UnitHeapBase() : mFreeNode(0) {}
    UnitHeapBase(size_t unit, uptr addr, size_t size, s32 alignment = DEFAULT_ALIGNMENT, bit32 option = 0) : mFreeNode(0) {
        Initialize(unit, addr, size, alignment, option);
    }

    void Initialize(size_t unit, uptr addr, size_t size, s32 alignment = DEFAULT_ALIGNMENT, bit32 option = 0);
    void Invalidate();
    void Finalize(){
        if(this->mFreeNode != 0){
            NN_TASSERT_(mCount == 0);
            this->mFreeNode = 0;
        }
    }
    void* Allocate(){
        void* ret = reinterpret_cast<void*&>(mFreeNode);
        if (ret){
            mFreeNode = mFreeNode->next;
            ++mCount;

            DebugFillMemory(reinterpret_cast<uptr>(ret), this->mUnit, HEAP_FILL_TYPE_ALLOC);
            FillMemoryZero(reinterpret_cast<uptr>(ret), this->mUnit);
        }

        return ret;
    }
    void Free(void* p){
        p = this->mFreeNode;
        mFreeNode = reinterpret_cast<Node*>(p);
        mCount--;
    }

    virtual ~UnitHeapBase(){ this->Finalize(); }
    virtual void FreeV(void* p){ this->Free(p); }
    virtual void* GetStartAddress() const{ return reinterpret_cast<void*>(this->mAddr); }
    virtual size_t GetTotalSize() const{ return mSize; }
    virtual void Dump() const ;
    virtual bool HasAddress(const void* addr) const{ return mAddr <= reinterpret_cast<uptr>(addr) && reinterpret_cast<uptr>(addr) < (mAddr + mSize);}

protected:
    bool IsFreeNode(uptr addr) const;

    struct Node {
        Node* next;
    };

    size_t mUnit;
    uptr   mAddr;
    size_t mSize;
    Node*  mFreeNode;
    s32    mAlignment;
    size_t mCount;

};

    inline void UnitHeapBase::Invalidate() {
        this->mFreeNode = 0;
    }

template <class LockPolicy>
class UnitHeapTemplate : public UnitHeapBase, private LockPolicy::LockObject {
private:
    typedef UnitHeapBase Base;
    typedef typename LockPolicy::LockObject LockObject;
    typedef typename LockPolicy::ScopedLock ScopedLock;

public:
    static size_t GetRequiredHeapSize(size_t unit, size_t numUnit, s32 alignment = DEFAULT_ALIGNMENT) {
        return Base::GetRequiredHeapSize(unit, numUnit, alignment);
    }

    UnitHeapTemplate() {}

    UnitHeapTemplate(size_t unit, uptr addr, size_t size, s32 alignment = DEFAULT_ALIGNMENT, bit32 option = 0) {
        Initialize(unit, addr, size, alignment, option);
    }

    static UnitHeapTemplate* Create(HeapBase* parent, size_t unit, void* addr, size_t size, s32 alignment = DEFAULT_ALIGNMENT, bit32 option = 0, bit32 placement = HEAP_INFOPLACEMENT_HEAD);

    void Initialize(size_t unit, uptr addr, size_t size, s32 alignment = DEFAULT_ALIGNMENT, bit32 option = 0) {
        Base::Initialize(unit, addr, size, alignment, option);
        LockObject::Initialize();
    }

    void Invalidate() {
        Base::Invalidate(); 
    }

    void Finalize() {
        LockObject::Finalize();
        Base::Finalize();
    }

    virtual ~UnitHeapTemplate(){
    }

    void* Allocate() {
        ScopedLock lk(*this);
        return Base::Allocate();
    }

    void Free(void* p) {
        ScopedLock lk(*this);
        Base::Free(p);
    }

    virtual void FreeV(void* p){
        Free(p); 
    }

    virtual void*  GetStartAddress() const {
        return Base::GetStartAddress(); 
    }

    virtual size_t GetTotalSize()    const {
        return Base::GetTotalSize(); 
    }

    virtual void Dump() const {
        ScopedLock lk(*this);
        Base::Dump();
    }

    virtual bool HasAddress(const void* addr) const {
        return Base::HasAddress(addr); 
    }
};

typedef UnitHeapTemplate<nn::os::LockPolicy::NoLock> UnitHeap;

typedef UnitHeapTemplate<nn::os::LockPolicy::Object<nn::os::CriticalSection> > ThreadSafeUnitHeap;

}
}
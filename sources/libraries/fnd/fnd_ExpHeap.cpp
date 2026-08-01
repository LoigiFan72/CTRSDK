// Filename: fnd_ExpHeap.cpp
//
// Project: Horizon

#include <nn/fnd/fnd_ExpHeap.h>
#include <nn/fnd/detail/fnd_DetailHeap.h>

namespace nn{
namespace fnd{

size_t ExpHeapBase::GetTotalSize() const{ 
    return (int)this->mExpHeapImpl.heapEnd - (int)this->mExpHeapImpl.heapStart; 
}

void* ExpHeapBase::GetStartAddress() const{ 
    return mExpHeapImpl.heapStart; 
}

bool ExpHeapBase::HasAddress(const void* addr) const{ 
    return mExpHeapImpl.heapStart <= addr && addr < mExpHeapImpl.heapEnd; 
}

void ExpHeapBase::Dump() const{
    
}

void ExpHeapBase::Invalidate() {
    void* obj;
    if (this->mExpHeapImpl.signature == 0){
        return;
    }
    nn::fnd::detail::RemoveListObject((detail::NNSFndList*)&this->mExpHeapImpl, obj);
    this->mExpHeapImpl.signature = 0;
}

void* ExpHeapBase::Allocate(size_t byteSize, s32 alignment, bit8 groupId, AllocationMode mode, bool reuse) {
    nn::fnd::detail::SetGroupIDForHelp((detail::Heap)&this->mExpHeapImpl, groupId);
    nn::fnd::detail::SetAllocModeForHeap((detail::Heap)&this->mExpHeapImpl, mode);
    nn::fnd::detail::UseMarginOfAlignmentForHeap((detail::Heap)&this->mExpHeapImpl, reuse);

    void* p = nn::fnd::detail::AllocFromHeap((detail::Heap)&this->mExpHeapImpl, byteSize, alignment);
    if (p) {
        this->mAllocCount++;
    }
    return p;
}

void ExpHeapBase::Initialize(uptr addr, size_t size, bit32 option){
    nn::fnd::detail::Heap newHeap;
    newHeap = nn::fnd::detail::CreateHeap((nn::fnd::detail::Heap)&this->mExpHeapImpl, (void*)addr, size, (ushort)option);
    if(newHeap == 0){
        nndbgPanic();
    }
    this->mAllocCount = 0;
}

void ExpHeapBase::FreeV(void* p){
    this->Free(p);
}

void ExpHeapBase::Free(void* p) {
    nn::fnd::detail::FreeToHeap((detail::Heap)&this->mExpHeapImpl, p);
    this->mAllocCount--;
}
    
}
}
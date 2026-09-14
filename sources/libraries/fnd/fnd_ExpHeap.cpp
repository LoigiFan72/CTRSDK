// Filename: fnd_ExpHeap.cpp
//
// Project: Horizon

#include <nn/fnd/fnd_ExpHeap.h>
#include <nn/fnd/detail/fnd_DetailHeap.h>

namespace nn{
namespace fnd{

size_t ExpHeapBase::GetTotalSize() const
{ 
    return (int)this->mExpHeapImpl.heapEnd - (int)this->m_ExpHeapImpl.heapStart; 
}

void* ExpHeapBase::GetStartAddress() const
{ 
    return mExpHeapImpl.heapStart; 
}

bool ExpHeapBase::HasAddress(const void* addr) const
{ 
    return m_ExpHeapImpl.heapStart <= addr && addr < m_ExpHeapImpl.heapEnd; 
}

void ExpHeapBase::Dump() const
{
    
}

void ExpHeapBase::Invalidate() 
{
    void* obj;
    if (this->m_ExpHeapImpl.signature == 0)
    {
        return;
    }
    nn::fnd::detail::RemoveListObject((detail::NNSFndList*)&this->m_ExpHeapImpl, obj);
    m_ExpHeapImpl.signature = 0;
}

void* ExpHeapBase::Allocate(size_t byteSize, s32 alignment, bit8 groupId, AllocationMode mode, bool reuse)
{
    nn::fnd::detail::SetGroupIDForHelp((detail::Heap)&this->m_ExpHeapImpl, groupId);
    nn::fnd::detail::SetAllocModeForHeap((detail::Heap)&this->m_ExpHeapImpl, mode);
    nn::fnd::detail::UseMarginOfAlignmentForHeap((detail::Heap)&this->m_ExpHeapImpl, reuse);

    void* p = nn::fnd::detail::AllocFromHeap((detail::Heap)&this->m_ExpHeapImpl, byteSize, alignment);
    if (p) 
    {
        this->m_AllocCount++;
    }
    return p;
}

void ExpHeapBase::Initialize(uptr addr, size_t size, bit32 option)
{
    nn::fnd::detail::Heap newHeap;
    newHeap = nn::fnd::detail::CreateHeap((nn::fnd::detail::Heap)&this->m_ExpHeapImpl, (void*)addr, size, (ushort)option);
    if(newHeap == 0)
    {
        nndbgPanic();
    }
    this->m_AllocCount = 0;
}

void ExpHeapBase::FreeV(void* p)
{
    this->Free(p);
}

void ExpHeapBase::Free(void* p) 
{
    nn::fnd::detail::FreeToHeap((detail::Heap)&this->m_ExpHeapImpl, p);
    this->m_AllocCount--;
}
    
}
}
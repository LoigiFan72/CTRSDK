// Filename: fnd_UnitHeap.cpp
//
// Project: Horizon

#include <nn/fnd/fnd_UnitHeap.h>

namespace nn{
namespace fnd{

void UnitHeapBase::Dump() const 
{
}

void UnitHeapBase::Initialize(size_t unit, uptr addr, size_t size, s32 alignment, bit32 option)
{
    NN_TASSERT_(this->mFreeNode == 0);
    NN_TASSERT_(alignment >= sizeof(void*));
    NN_TASSERT_(unit >= sizeof(void*));
    NN_TASSERT_(alignment % sizeof(void*) == 0);
    HeapBase::Initialize(option);
    this->m_Unit = RoundUp(unit, alignment);
    this->m_Addr = RoundUp(addr, alignment);
    this->m_Size = RoundDown((addr+size)-m_Addr, m_Unit);
    this->m_Alignment = alignment;
    this->m_Count = 0;

    this->DebugFillMemory(addr, size, HEAP_FILL_TYPE_NOUSE);
    
    Node* freeNode = 0;
    for (uptr addr2 = m_Addr + m_Size - m_Unit; addr2 >= m_Addr; addr2 -= m_Unit)
    {
        reinterpret_cast<Node*>(addr2)->next = freeNode;
        freeNode = reinterpret_cast<Node*>(addr2);
    }
    NN_TASSERT_(reinterpret_cast<uptr>(freeNode) == m_Addr || freeNode == 0);
    this->m_FreeNode = freeNode;
}


}
}
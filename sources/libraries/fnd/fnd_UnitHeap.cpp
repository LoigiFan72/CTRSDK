// Filename: fnd_UnitHeap.cpp
//
// Project: Horizon

#include <nn/fnd/fnd_UnitHeap.h>

namespace nn{
namespace fnd{

void UnitHeapBase::Dump() const {

}

void UnitHeapBase::Initialize(size_t unit, uptr addr, size_t size, s32 alignment, bit32 option){
    NN_TASSERT_(this->mFreeNode == 0);
    NN_TASSERT_(alignment >= sizeof(void*));
    NN_TASSERT_(unit >= sizeof(void*));
    NN_TASSERT_(alignment % sizeof(void*) == 0);
    HeapBase::Initialize(option);
    this->mUnit = RoundUp(unit, alignment);
    this->mAddr = RoundUp(addr, alignment);
    this->mSize = RoundDown((addr+size)-mAddr, mUnit);
    this->mAlignment = alignment;
    this->mCount = 0;

    this->DebugFillMemory(addr, size, HEAP_FILL_TYPE_NOUSE);
    
    Node* freeNode = 0;
    for (uptr addr2 = mAddr + mSize - mUnit; addr2 >= mAddr; addr2 -= mUnit){
        reinterpret_cast<Node*>(addr2)->next = freeNode;
        freeNode = reinterpret_cast<Node*>(addr2);
    }
    NN_TASSERT_(reinterpret_cast<uptr>(freeNode) == mAddr || freeNode == 0);
    this->mFreeNode = freeNode;
}


}
}
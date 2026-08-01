// Filename: fnd_HeapBase.cpp
//
// Project: Horizon

#include <nn/fnd/fnd_HeapBase.h>
#include <nn/dbg/dbg_Break.h>

namespace nn{
namespace fnd{

HeapBase::~HeapBase(){
}

void HeapBase::FillMemory32(uptr begin, uptr end, bit32 v){
    bit32*& p = reinterpret_cast<bit32*&>(begin);
    bit32*& q = reinterpret_cast<bit32*&>(end);
    while (p != q){
        *p++ = v;
    }
}

}
}
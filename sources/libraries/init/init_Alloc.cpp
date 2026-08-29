// Filename: init_Alloc.cpp
//
// Project: Horizon

#include <nn/init/init_Alloc.h>
#include <nn/fnd/fnd_ExpHeap.h>
#include <nn/os/os_MemoryBlock.h>

#include <nn/dbg/dbg_Break.h>
#include <nn/Assert.h>

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <new>
#include <cstring>

typedef nn::fnd::ExpHeapTemplate<nn::os::LockPolicy::Object<nn::os::CriticalSection> > SystemExpHeap;

namespace{
    SystemExpHeap* s_pSystemHeap = 0;
    nn::util::aligned_storage<sizeof(SystemExpHeap::Allocator), nn::util::alignment_of<SystemExpHeap::Allocator>::value >::type s_SystemAllocatorBuffer;
    SystemExpHeap::Allocator* s_pSystemAllocator = 0;
    nnosMemoryBlock s_HeapMemoryBlock;
}

namespace nn{
namespace init{

void InitializeAllocator(uptr addr, size_t size){
    const size_t alignment = nn::util::alignment_of<SystemExpHeap>::value;
    uptr heapAddr = (((addr - 1) / alignment) + 1) * alignment;
    uptr headAddr = heapAddr + sizeof(SystemExpHeap);
    s_pSystemHeap = new (reinterpret_cast<void*>(heapAddr)) SystemExpHeap(headAddr, addr + size - headAddr);
    s_pSystemAllocator = new (&s_SystemAllocatorBuffer) SystemExpHeap::Allocator(*s_pSystemHeap);
}

void InitializeAllocator(size_t size){
    nnosMemoryBlockAllocate(&s_HeapMemoryBlock, size);
    InitializeAllocator(nnosMemoryBlockGetAddress(&s_HeapMemoryBlock), size);
}

}
}

extern "C" {
__weak void* malloc(std::size_t size){
    return s_pSystemHeap->Allocate(size,4,0,nn::fnd::ExpHeapBase::ALLOCATION_MODE_FIRST_FIT,false);
}

void free(void* ptr){
    if (ptr){
        s_pSystemHeap->Free(ptr);
    }
}
}

void* operator new(unsigned int size) {
    return std::malloc(size);
}

void* operator new[](size_t size, std::nothrow_t const&){
    return operator new(size);
}

__weak void operator delete (void* p) throw(){
    std::free(p);
}

__weak void operator delete[] (void* p) throw(){
    operator delete(p);
}
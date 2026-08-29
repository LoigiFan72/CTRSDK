// Filename: os_MemoryBlock.cpp
//
// Project: Horizon

#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_AddressSpaceManager.h>
#include <nn/dbg/dbg_Break.h>

namespace nn{
namespace os{
    nnosAddressSpaceManager s_SpaceManager;

namespace detail{

bool IsMemoryBlockEnabled(){
    return s_IsMemoryBlockEnabled;
}

uptr AllocateFromMemoryBlockSpace(MemoryBlockBase* p, size_t size){
    AddressSpaceManager* pManager;
    pManager->Allocate(p,size,0);
}

void FreeToMemorySpace(MemoryBlockBase* p){
    AddressSpaceManager* pManager;
    pManager->Free(p);
}

}

void InitializeMemoryBlock(uptr begin, size_t size){
    if(sIsMemoryBlockEnabled == 0){
        sIsMemoryBlockEnabled = 1;
        nnosAddressSpaceManagerInitialize(&s_SpaceManager,begin,size);
        return;
    }
    return;
}

void Switch(nnosMemoryBlockBase* pTo, nnosMemoryBlockBase* pFrom){
    nnosAddressSpaceManagerSwitch(&s_SpaceManager,pTo,pFrom);
}
}
}

void nnosMemoryBlockAllocate(nnosMemoryBlock* p, size_t size){

}

uptr nnosMemoryBlockGetAddress(nnosMemoryBlock* p){
    return *(uptr*)(p->buf + 8);
}
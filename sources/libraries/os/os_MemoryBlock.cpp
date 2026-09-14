// Filename: os_MemoryBlock.cpp
//
// Project: Horizon

#include <nn/assert.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/os/os_Memory.h>
#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_Result.h>
#include <nn/os/os_ErrorHandlerSelect.h>
#include <nn/svc/svc_Stub.h>
#include <nn/util/util_Result.h>

#include "os_AddressSpaceManager.h"

namespace nn{
namespace os{
namespace{
    bool s_IsMemoryBlockEnabled;
    nnosAddressSpaceManager s_SpaceManager;
}

namespace detail{

bool IsMemoryBlockEnabled()
{
    return s_IsMemoryBlockEnabled;
}

uptr AllocateFromMemoryBlockSpace(MemoryBlockBase* p, size_t size)
{
    AddressSpaceManager* pManager;
    pManager->Allocate(p,size,0);
}

void Switch(MemoryBlock* pTo, MemoryBlock* pFrom)
{
    nnosAddressSpaceManagerSwitch(&s_SpaceManager, (nnosMemoryBlockBase*)pTo, (nnosMemoryBlockBase*)pFrom);
}

void FreeToMemoryBlockSpace(MemoryBlockBase* p)
{
    AddressSpaceManager* pManager = reinterpret_cast<AddressSpaceManager*>(&s_SpaceManager);
    pManager->Free(p);
}

}

void MemoryBlock::Initialize(size_t size)
{
    NN_TASSERTMSG_(os::detail::IsMemoryBlockEnabled(), "InitializeMemoryBlock is not called.\n");
    NN_TASSERTMSG_(GetAddress() == 0, "This MemoryBlock instance has been already initialized.\n");
    if (!os::detail::IsMemoryBlockEnabled() || GetAddress() != 0)
    {
        return;
    }
    
    size = GetPageAlignedSize(size);
    uptr addr = os::detail::AllocateFromMemoryBlockSpace(this, size);
    if (addr == 0)
    {
        NN_OS_ERROR_IF_FAILED(ResultNoAddressSpace());
    }
    this->MemoryBlockBase::SetReadOnly(false);
}

void MemoryBlock::Finalize()
{
    if (GetAddress())
    {
        os::detail::FreeToMemoryBlockSpace(this);
    }
}

void InitializeMemoryBlock(uptr begin, size_t size){
    if(s_IsMemoryBlockEnabled == false)
    {
        s_IsMemoryBlockEnabled = true;
        nnosAddressSpaceManagerInitialize(&s_SpaceManager,begin,size);
        return;
    }
}

}
}

#include <new>
using namespace nn::os;

extern "C" 
{

void nnosMemoryBlockAllocate(nnosMemoryBlock* p, size_t size)
{
    NN_TASSERT_(os::detail::IsMemoryBlockEnabled());
    new (p) nn::os::MemoryBlock(size);
}

uptr nnosMemoryBlockGetAddress(nnosMemoryBlock* p)
{
    nn::os::MemoryBlock* pMemoryBlock = reinterpret_cast<nn::os::MemoryBlock*>(p);
    return pMemoryBlock->GetAddress();
}

}
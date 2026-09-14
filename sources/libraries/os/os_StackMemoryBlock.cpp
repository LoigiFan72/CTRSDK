// Filename: os_StackMemoryBlock.cpp
//
// Project: Horizon

#include <nn/os/os_StackMemoryBlock.h>
#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_Result.h>
#include <nn/assert.h>
#include <nn/svc/svc_Stub.h>
#include <nn/util/util_Result.h>
#include <nn/os/os_Memory.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/os/os_ErrorHandlerSelect.h>

#include "os_AddressSpaceManager.h"

namespace nn{
namespace os{
namespace{
    nnosAddressSpaceManager s_SpaceManager;
}

namespace detail{

void InitializeStackMemory()
{
    nnosAddressSpaceManagerInitialize(&s_SpaceManager,0xe000000, 0x2000000);
}

void Switch(nnosMemoryBlockBase* pTo, nnosMemoryBlockBase* pFrom)
{
    nnosAddressSpaceManagerSwitch(&s_SpaceManager,pTo,pFrom);
}

} // detail

void StackMemoryBlock::Initialize(size_t size)
{
    NN_TASSERTMSG_(os::detail::IsMemoryBlockEnabled(), "InitializeMemoryBlock is not called.\n");
    NN_TASSERTMSG_(GetAddress() == 0, "This StackMemoryBlock instance has been already initialized.\n");
    if ( !os::detail::IsMemoryBlockEnabled() || GetAddress() != 0 )
    {
        return;
    }

    size = GetPageAlignedSize(size);

    uptr addr = os::detail::AllocateFromMemoryBlockSpace(this, size);
    if (addr == NULL)
    {
        NN_OS_ERROR_IF_FAILED(ResultNoAddressSpace());
    }
}

}
}

using namespace nn::os;

void nnosStackMemoryBlockAllocate(nnosStackMemoryBlock* p, size_t size)
{
    NN_TASSERT_(os::detail::IsMemoryBlockEnabled());
    new (p) StackMemoryBlock(size);
}

void nnosStackMemoryBlockFree(nnosStackMemoryBlock* p)
{
    StackMemoryBlock* pStackMemoryBlock = reinterpret_cast<StackMemoryBlock*>(p);
    pStackMemoryBlock->~StackMemoryBlock();
}

uptr nnosStackMemoryBlockGetStackBottom(nnosStackMemoryBlock* p)
{
    StackMemoryBlock* pStackMemoryBlock = reinterpret_cast<StackMemoryBlock*>(p);
    return pStackMemoryBlock->GetStackBottom();
}

void nnosStackMemoryBlockInitialize(nnosStackMemoryBlock* p)
{
    new (p) StackMemoryBlock();
}
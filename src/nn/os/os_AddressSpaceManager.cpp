// Filename: os_AddressSpaceManager.cpp
//
// Project: Horizon Decompilation

#include <nn/os/os_AddressSpaceManager.h>
#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/fnd/fnd_Intrusive.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/Assert.h>

namespace nn{
namespace os{

void AddressSpaceManager::Initialize(uptr begin, size_t size){
    if (mSpaceBegin == 0 && mSpaceEnd == 0){
        this->mLock.Initialize();
        mSpaceBegin = begin;
        mSpaceEnd = begin + size;
    }
}

uptr AddressSpaceManager::Allocate(MemoryBlockBase* pBlock, size_t size, size_t skipSize){
    NN_NULL_TASSERT_(pBlock);
    NN_ALIGN_TASSERT_(size, NN_OS_MEMORY_PAGE_SIZE);
    NN_ALIGN_TASSERT_(skipSize, NN_OS_MEMORY_PAGE_SIZE);
    Lock::ScopedLock scopedLock(this->mLock);

    MemoryBlockBase* pPrev = FindSpace(size, skipSize);
    uptr allocatedAddress;

    if(pPrev != NULL){
        allocatedAddress = pPrev->GetAddress() + pPrev->GetSize() + skipSize;
        MemoryBlockBase* pNext = mBlockList.GetNext(pPrev);

        if(pNext != NULL){
            mBlockList.Insert(pNext, pBlock);
        }
        else{
            mBlockList.PushBack(pBlock);
        }
    }
    else{
        allocatedAddress = mSpaceBegin;
        MemoryBlockBase* pNext = mBlockList.GetFront();

        if(pNext != NULL){
            const uptr allocatedEnd = allocatedAddress + size;
            const uptr nextBegin    = pNext->GetAddress();

            if(nextBegin < allocatedEnd + skipSize){
                return NULL;
            }

            mBlockList.Insert(pNext, pBlock);
        }
        else{
            const uptr allocatedEnd = allocatedAddress + size;

            if(mSpaceEnd < allocatedEnd){
                return NULL;
            }
            mBlockList.PushBack(pBlock);
        }
    }

    pBlock->SetAddressAndSize(allocatedAddress, size);
    return allocatedAddress;
}

void AddressSpaceManager::Free(MemoryBlockBase *pBlock){
    Lock::ScopedLock scopedLock(this->mLock);
    this->mBlockList.Erase(pBlock);
    pBlock->SetAddressAndSize(NULL, 0);
}

void AddressSpaceManager::Switch(MemoryBlockBase *pTo,MemoryBlockBase *pFrom){
    Lock::ScopedLock scopedLock(this->mLock);

    pTo->SetAddressAndSize(pFrom->GetAddress(), pFrom->GetSize());
    this->mBlockList.Insert(pFrom, pTo);

    pFrom->SetAddressAndSize(NULL, 0);
    this->mBlockList.Erase(pFrom);
}

MemoryBlockBase* AddressSpaceManager::FindSpace(size_t size, size_t skipSize){
    MemoryBlockBase* pItem = this->mBlockList.GetBack();
    uptr end = mSpaceEnd;

    while(pItem != NULL){
        const uptr nextBegin = pItem->GetAddress();
        const uptr nextEnd = nextBegin + pItem->GetSize();
        const size_t spaceSize = end - nextEnd;
        if(spaceSize >= size + skipSize){
            return pItem;
        }

        end = nextBegin - skipSize;
        pItem = this->mBlockList.GetPrevious(pItem);
    }
    return NULL;
}

}
}

using namespace nn::os;

// TODO: Remake these.

extern "C" {

void nnosAddressSpaceManagerInitialize(nnosAddressSpaceManager* p, uptr begin, size_t size){
    AddressSpaceManager* pThis = new (p) AddressSpaceManager();
    pThis->Initialize(begin, size);
}

// Pointer to -> AddressSpaceMan::Switch
void nnosAddressSpaceManagerSwitch(nnosAddressSpaceManager* p, nnosMemoryBlockBase* p2, nnosMemoryBlockBase* p3){
    nn::os::AddressSpaceManager* pThis = reinterpret_cast<nn::os::AddressSpaceManager*>(p);
    nn::os::MemoryBlockBase* pTo = reinterpret_cast<nn::os::MemoryBlockBase*>(p2);
    nn::os::MemoryBlockBase* pFrom = reinterpret_cast<nn::os::MemoryBlockBase*>(p3);
    pThis->Switch(pTo, pFrom);
}

}
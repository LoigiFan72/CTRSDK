#pragma once

#include <nn/Handle.h>
#include <nn/ro/ro_RegistrationList.h>
#include <nn/ro/ro_Module.h>
#include <nn/ro/ro_Types.h>

namespace nn{
namespace ro{
    Result Initialize(void* pRs,uint rsSize);
    Result Finalize();
    RegistrationList* RegisterList(void* pRr, size_t rrSize);
    Module* FindModule(const char* pName);
    uint GetAddress(const char* pAddress);
    void GetSizeInfo(SizeInfo* pInfo,void* pBuffer);
    Module* LoadModule(void* pRo,size_t roSize,void* pBuffer,size_t  bufferSize,bool doRegister,FixLevel fixLevel,const RegistrationList* pRr);

namespace detail{
    const char PORT_NAME_RELOCATEABLE_OBJECT[] = "ldr:ro";
    const s32 ENTRY_NOT_FOUND = -1;

    uptr GetOriginalAddress(const void* p);
    void UpdateRegistrationListNode(nn::ro::RegistrationList* p);
    s32 FindRegistrationListEntry(const nn::ro::RegistrationList** superP, const void* p);
    void* GetRoot();

    __weak bool IsCodeAddress(uptr addr);
}
}

}
// Filename: ro_Api.cpp
//
// Project: Horizon

#include <nn/ro/ro_Api.h>
#include <nn/srv.h>
#include <nn/svc.h>
#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/os.h>
#include <nn/err.h>
#include <nn/math.h>
#include <nn/util/detail/util_Symbol.h>
#include <nn/ro.h>
#include <nn/ro/ro_Info.h>
#include <nn/ro/ro_DynamicLoader.h>
#include <nn/ro/ro_ApiException.h>
#include <nn/ro/ro_DebugInfo.h>
#include <nn/dbg/dbg_PrintResult.h>
#include <nn/ro/ro_ObjectFile.h>
#include <nn/module.h>
#include <algorithm>

extern "C"{
    extern bit8 Image$$STATIC_END$$Base[];
    nnResult nnRoDetailInitializeLinkException(void* pRs, size_t rsSize);
}

namespace nn{
namespace ro{
namespace{
    NN_MAKE_MODULE(sModuleSign, "NINTENDO", "RO"); // found in plainrgn.bin

    ModuleHeader* spStatic;
    ModuleRegistrationListHeader* spListNode;

    Result Connect(){
        return srv::GetServiceHandle(&detail::DynamicLoader::sSession, "ldr:ro");
    }

    void Disconnect(){
        Result result = svc::CloseHandle(detail::DynamicLoader::sSession);
        NN_ERR_THROW_FATAL_ALL(result);

        detail::DynamicLoader::sSession = INVALID_HANDLE_VALUE;
    }

    bool IsConnected(){
        return detail::DynamicLoader::sSession != INVALID_HANDLE_VALUE;
    }

    uptr GetLocateAddress(void* p){
        const uptr orgAddr = reinterpret_cast<uptr>(p);
        const uptr heapBase = os::detail::GetHeapAddressWithoutCheck();

        if(orgAddr >= heapBase){
            return orgAddr - (heapBase - reinterpret_cast<uptr>(Image$$STATIC_END$$Base));
        }
        else{
            return orgAddr;
        }
    }

    Result InitializeImpl(void* pRs, size_t rsSize){
        Result res;

        NN_REFER_MODULE(sModuleSign);

        if(IsConnected()){
            return ro::ResultAlreadyInitialized();
        }

        res = Connect();
        if(res.IsFailure()){
            return res;
        }

        const uptr locateAddr = GetLocateAddress(pRs);

        res = detail::DynamicLoader::Startup(PSEUDO_HANDLE_CURRENT_PROCESS,reinterpret_cast<uptr>(pRs),rsSize,locateAddr);

        if(res.IsSuccess()){
            spStatic = reinterpret_cast<ModuleHeader*>(locateAddr);
            detail::EnableDebugNotification(false);
        }
        else{
            Disconnect();
        }

        return res;
    }

    s32 FindHash(const ModuleRegistrationListHeader& rrHeader, const Hash& hash){
        Hash* pHashList = rrHeader.hashOffset.GetPointer(&rrHeader);
        Hash* pHashListEnd = pHashList + rrHeader.numHash;
        Hash* pFound = std::lower_bound(pHashList, pHashListEnd, hash);

        return (pFound != pHashListEnd) ? pFound - pHashList: detail::ENTRY_NOT_FOUND;
    }

    bool HasDebugInfo(const ModuleRegistrationListHeader& rrHeader){
        return rrHeader.debugInfoSize != 0;
    }

    Result UnloadActiveModules(ModuleHeader* pHead){
        if(pHead != NULL){
            ModuleHeader* pTail;

            do{
                pTail = pHead->node.pPrev;
                Result res = reinterpret_cast<Module*>(pTail)->Unload();

                if(res.IsFailure()){
                    return res;
                }

            } while(pTail != pHead);
        }

        return ResultSuccess();
    }

    Result UnloadModule(ModuleHeader* pRoot){
        Result res;

        res = UnloadActiveModules(pRoot->node.pNext);
        if(res.IsFailure()){
            return res;
        }

        res = UnloadActiveModules(pRoot->node.pPrev);
        if(res.IsFailure()){
            return res;
        }

        return res;
    }

    Result UnregisterAllRegistrationList(ModuleRegistrationListHeader* pAny){
        Result res;

        ModuleRegistrationListHeader* p = pAny;

        while(p->node.pPrev != NULL){
            p = p->node.pPrev;
        }

        while(p != NULL){
            ModuleRegistrationListHeader* pNext = p->node.pNext;

            res = reinterpret_cast<RegistrationList*>(p)->Unregister();

            if(res.IsFailure()){
                return res;
            }

            p = pNext;
        }

        return ResultSuccess();
    }
}

namespace detail{

uptr GetOriginalAddress(const void* p){
    const uptr locateAddr = reinterpret_cast<uptr>(p);
    if(locateAddr >= reinterpret_cast<uptr>(Image$$STATIC_END$$Base)){
        const uptr heapBase = os::detail::GetHeapAddressWithoutCheck();
        return locateAddr + (heapBase - reinterpret_cast<uptr>(Image$$STATIC_END$$Base));
    }
    else{
        return locateAddr;
    }
}

void UpdateRegistrationListNode(RegistrationList* p){
    spListNode = reinterpret_cast<ModuleRegistrationListHeader*>(p);
}

s32 FindRegistrationListEntry(const RegistrationList** pp, const void* p){
    const Hash& hash = *reinterpret_cast<const Hash*>(p);

    for(ModuleRegistrationListHeader* prlh = spListNode; prlh != NULL; prlh = prlh->node.pNext){
        const s32 found = FindHash(*prlh, hash);

        if(found != -1){
            *pp = reinterpret_cast<RegistrationList*>(prlh);
            return found;
        }
    }

    return -1;
}

void* GetRoot(){
    return spStatic;
}

bool IsCodeAddress(uptr addr){
    class CodeChecker : public ro::Module::EnumerateCallback{
    private:
        uptr mAddress;
        bool mResult;
        bool mEnabled;
        short rev;
    public:
        CodeChecker(uptr addr): 
            mAddress(addr), 
            mResult(false), 
            mEnabled(false)
        {}
        virtual bool operator()(ro::Module* p){
            mEnabled = true;

            ro::RegionInfo ri;
            p->GetRegionInfo(&ri);
            uptr codeEnd = ri.mCodeBegin + ri.mCodeSize;

            if((ri.mCodeBegin <= mAddress)&& (mAddress < codeEnd) ){
                mResult = true;
                return false;
            }
            else{
                return true;
            }
        }
        bool GetResult() const { return mResult; }
        bool IsEnabled() const { return mEnabled; }
    } checker(addr);

    if(checker.IsEnabled()){
        return checker.GetResult();
    }
    else{
        const uptr begin = os::GetCodeRegionAddress();
        const uptr end = begin + os::GetCodeRegionSize();
        return (begin <= addr) && (addr < end);
    }
}

} // detail

Result Initialize(void* pRs, size_t rsSize){
    NN_UTIL_REFER_SYMBOL(ro::detail::IsCodeAddress);
    return nnRoDetailInitializeLinkException(pRs, rsSize);
}

Result Finalize(){
    Result res = ResultSuccess();

    if(!IsConnected()){
        return ResultNotInitialized();
    }

    if(spStatic != NULL){
        res = UnloadModule(spStatic);
    }

    if(res.IsSuccess()){
        if(spListNode != NULL){
            res = UnregisterAllRegistrationList(spListNode);
        }
    }

    if(res.IsSuccess()){
        res = detail::DynamicLoader::Cleanup(PSEUDO_HANDLE_CURRENT_PROCESS,detail::GetOriginalAddress(spStatic));
    }
    if(res.IsSuccess()){
        spStatic = NULL;
        Disconnect();
    }

    return res;
}

RegistrationList* RegisterList(void* pRr, size_t rrSize){
    Result res;

    res = detail::DynamicLoader::RegisterList(PSEUDO_HANDLE_CURRENT_PROCESS,reinterpret_cast<uptr>(pRr),rrSize);

    if(res.IsSuccess()){
        RegistrationList* p = reinterpret_cast<RegistrationList*>(pRr);
        detail::UpdateRegistrationListNode(p);

        if(HasDebugInfo(*reinterpret_cast<ModuleRegistrationListHeader*>(pRr))){
            detail::EnableDebugNotification(true);
        }

        return p;
    }
    else{
        return NULL;
    }
}

Result GetSizeInfo(SizeInfo* pInfo, const void* pRo){
    return detail::GetSizeInfo(pInfo, pRo);
}

size_t CalculateRequiredBufferSize(const void* pRo, size_t roSize){
    size_t size;
    bool ok = detail::GetBssSize(&size, pRo);
    return ok ? size: 0;
}

Module* FindModule(const char* pName){
    void* p;
    bool ok = detail::FindModule(&p, spStatic, pName);
    return ok ? reinterpret_cast<Module*>(p): NULL;
}

// hey guys, SuperHorrorBro mike here and in todays video...
//
// I like SHB :D

Module* LoadModule(void* pRo,size_t roSize,void* pBuffer,size_t  bufferSize,bool doRegister,FixLevel fixLevel,const RegistrationList* pRr){    
    Result res;

    const ModuleHeader& header = *reinterpret_cast<const ModuleHeader*>(pRo);

    if(header.signature != detail::SIGNATURE_RO){
        if(header.signature == detail::SIGNATURE_RO_FIXED){
            NN_TWARNING_(false, "Cannot load FIXed CRO0.\n");
        }

        else{
            NN_TWARNING_(false, "Invalid signature.\n");
        }

        return NULL;
    }

    const uptr      locateCodeAddr  = GetLocateAddress(pRo);
    const uptr      locateDataAddr  = NULL;
    const uptr      dataBufferAddr  = reinterpret_cast<uptr>(pBuffer);
    const size_t    dataBufferSize  = math::RoundUp(header.heapBinarySize, 4);
    const uptr      bssBufferAddr   = dataBufferAddr + dataBufferSize;
    const size_t    bssBufferSize   = bufferSize - dataBufferSize;

    size_t fixedSize;

    res = detail::DynamicLoader::Load(&fixedSize,PSEUDO_HANDLE_CURRENT_PROCESS,reinterpret_cast<uptr>(pRo),locateCodeAddr,
                roSize,dataBufferAddr,locateDataAddr,dataBufferSize,bssBufferAddr,bssBufferSize,doRegister,fixLevel,reinterpret_cast<uptr>(pRr) );

    if(res.IsFailure()){
        return NULL;
    }

    ModuleHeader* pHeader = reinterpret_cast<ModuleHeader*>(locateCodeAddr);

    if(pHeader->heapBinarySize > 0){
        const uptr codeDataBegin = pHeader->heapBinary.operator uptr();

        const uptr orgCodeBegin = reinterpret_cast<uptr>(pRo);
        const uptr orgCodeEnd   = orgCodeBegin + fixedSize;

        const uptr   orgDataBegin = reinterpret_cast<uptr>(pRo) + (codeDataBegin - locateCodeAddr);
        const size_t orgDataSize  = pHeader->heapBinarySize;

        size_t dataSizeInCode = 0;

        if(orgDataBegin < orgCodeEnd){
            dataSizeInCode = math::Min(orgCodeEnd - orgDataBegin, orgDataSize);
        }

        if(orgDataSize > dataSizeInCode){
            std::memmove(reinterpret_cast<void*>(dataBufferAddr + dataSizeInCode),reinterpret_cast<void*>(orgDataBegin + dataSizeInCode), orgDataSize - dataSizeInCode);
        }

        if(dataSizeInCode > 0){
            std::memcpy(reinterpret_cast<void*>(dataBufferAddr),reinterpret_cast<void*>(codeDataBegin),dataSizeInCode);
        }
    }

    std::memset(reinterpret_cast<bit8*>(bssBufferAddr), 0, bssBufferSize);

    detail::SetupWork(pHeader, pBuffer, bufferSize);

    detail::RegisterEit(reinterpret_cast<Module*>(locateCodeAddr));

    Module* pModule = reinterpret_cast<Module*>(locateCodeAddr);
    detail::NotifyDllLoadedToDebugger(pModule);

    return pModule;
}

}
}

extern "C"{
    nnResult nnRoDetailInitializeImpl(void* pRs, size_t rsSize){
        return nn::ro::InitializeImpl(pRs, rsSize);
    }
}
// Filename: ro_Module.cpp
//
// Project: Horizon

#include <nn/srv.h>
#include <nn/svc.h>
#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/os.h>
#include <nn/math.h>
#include <nn/ro.h>
#include <nn/ro/ro_DynamicLoader.h>
#include <nn/ro/ro_Info.h>
#include <nn/ro/ro_ApiException.h>
#include <nn/ro/ro_DebugInfo.h>

namespace nn {
namespace ro {
namespace{
    ModuleHeader* GetRootHeader(){
        return reinterpret_cast<ModuleHeader*>(detail::GetRoot());
    }
    
    bool EnumerateModules(ModuleHeader* pHead, Module::EnumerateCallback* p){
        while(pHead != NULL){
            bool bContinue = p->operator ()(reinterpret_cast<Module*>(pHead));

            if(!bContinue){
                return false;
            }

            pHead = pHead->node.pNext;
        }

        return true;
    }

}

Result Module::Unload(){
    Result result;

    detail::NotifyDllUnloadingToDebugger(this);
    detail::UnregisterEit(this);

    NN_DBG_CHECK_RESULT(detail::ControlObject(this, NULL, detail::OBJECT_CONTROL_SET_WORK_AREA));

    void* pCurDataBegin = NULL;
    void* pOrgDataBegin = NULL;
    size_t orgDataSize  = 0;

    {
        const ModuleHeader& header = *reinterpret_cast<ModuleHeader*>(this);

        if(header.signature == detail::SIGNATURE_RO){

            {
                const SectionInfo* pSectionTable = header.sectionInfo;

                for(int i = 0; i < header.numSections; ++i){
                    const SectionInfo& si = pSectionTable[i];
                    if(si.section == SECTION_RW){
                        pCurDataBegin = reinterpret_cast<void*>(si.offset.operator uptr());
                        break;
                    }
                }
            }

            if(pCurDataBegin != NULL){
                const uptr orgBegin = detail::GetOriginalAddress(this);
                const uptr codeDataBegin = header.heapBinary.operator uptr();
                const uptr locateCodeAddr = reinterpret_cast<uptr>(this);
                pOrgDataBegin = reinterpret_cast<bit8*>(orgBegin + (codeDataBegin - locateCodeAddr));

                if(pOrgDataBegin != pCurDataBegin){
                    orgDataSize  = header.heapBinarySize;
                }
            }
        }
    }

    const size_t roSize = 0;

    result = detail::DynamicLoader::Unload(PSEUDO_HANDLE_CURRENT_PROCESS,this->GetHead(),roSize,detail::GetOriginalAddress(this));

    if(result.IsSuccess()){
        if(orgDataSize > 0){
            std::memcpy(pOrgDataBegin, pCurDataBegin, orgDataSize);
        }

        uptr base = detail::GetOriginalAddress(this);
        ModuleHeader& header = *reinterpret_cast<ModuleHeader*>(base);

        for(int i = 0; i < header.numSections; ++i){
            SectionInfo& si = header.sectionInfo.GetPointer(base)[i];
            switch(si.section){
            case SECTION_RW:   
                si.offset.SetPointer(&*header.heapBinary);  
                break;
            case SECTION_ZI:   
                si.offset.SetPointer(NULL); 
                break;
            }
        }
    }

    return result;
}

void Module::GetRegionInfo(RegionInfo* pri){
    detail::GetRegionInfo(pri, this);
}


void Module::Enumerate(EnumerateCallback* p){
    ModuleHeader* pRoot = GetRootHeader();

    if(pRoot != NULL){
        bool bContinue = EnumerateModules(pRoot->node.pNext, p);

        if(bContinue){
            EnumerateModules(pRoot->node.pPrev, p);
        }
    }
}

}
}
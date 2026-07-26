// Filename: ro_DebugInfo.cpp
//
// Project: Horizon

#include <nn/ro.h>
#include <nn/dbg.h>
#include <nn/ro/ro_Info.h>
#include <nn/ro/ro_Api.h>
#include <nn/ro/ro_ObjectFile.h>
#include <nn/ro/ro_DebugInfo.h>

namespace nn {
namespace ro {
namespace detail {

    using ro::ModuleHeader;
    using dbg::detail::CTR::DllInfo;

namespace{

    bool sDebugNotificationEnabled = false;

    const char16* GetPath(s32* pPathLength, const Hash& hash){
        const RegistrationList* prl;
        const s32 found = detail::FindRegistrationListEntry(&prl, &hash);

        if(found < 0){
            return NULL;
        }

        const ModuleRegistrationListHeader& header = *reinterpret_cast<const ModuleRegistrationListHeader*>(prl);

        if(header.debugInfoSize <= 0){
            return NULL;
        }

        const DebugInfoHeader& dih      = *header.debugInfoOffset.GetPointer(&header); // dihh
        const DebugInfoMapEntry* pdim   = dih.tableOffset.GetPointer(&header);
        const s32 numEntry              = dih.numTableEntry;

        if(found >= numEntry ){
            return NULL;
        }

        const DebugInfoMapEntry& dim    = pdim[found];
        const DebugInfoBody& dib        = *dim.bodyOffset.GetPointer(&header);

        *pPathLength = dib.pathLength;
        return dib.pathOffset.GetPointer(&header);
    }

    const char16* GetPath(s32* pPathLength, const ModuleHeader& header){
        Hash hash;
        Result result;

        crypto::CalculateSha256(&hash, &header.ModuleHash, sizeof(header.ModuleHash));
        return GetPath(pPathLength, hash);
    }

    uptr GetRwAddress(const ModuleHeader& header){
        for(s32 i = 0; i < header.numSections; ++i){
            const SectionInfo& si = header.sectionInfo[i];

            if(si.section == SECTION_RW){
                return si.offset;
            }
        }

        return NULL;
    }

    bool MakeDllInfo(DllInfo* pInfo, const ModuleHeader& header){
        s32 pathLength;
        const char16* pPath = GetPath(&pathLength, header);
        if(pPath == NULL){
            return false;
        }

        pInfo->pathAddress  = reinterpret_cast<uptr>(pPath);
        pInfo->pathLength   = pathLength + 1;
        pInfo->erAddress    = reinterpret_cast<uptr>(&header);
        pInfo->rwAddress    = GetRwAddress(header);

        return true;
    }
}

void NotifyDllLoadedToDebugger(const Module* pModule){
    if(sDebugNotificationEnabled ){
        dbg::detail::CTR::DllInfo dllInfo;

        if(MakeDllInfo(&dllInfo, *reinterpret_cast<const ModuleHeader*>(pModule))){
            dbg::detail::NotifyDllLoadedToDebugger(&dllInfo, sizeof(dllInfo));
        }
        else{
            NN_TLOG_("ro: debug information for module \"%s\" is not registered.\n", pModule->GetName());
        }
    }
}

void NotifyDllUnloadingToDebugger(const Module* pModule){
    if(sDebugNotificationEnabled){
        dbg::detail::CTR::DllInfo dllInfo;

        if(MakeDllInfo(&dllInfo, *reinterpret_cast<const ModuleHeader*>(pModule))){
            dbg::detail::NotifyDllUnloadingToDebugger(&dllInfo, sizeof(dllInfo));
        }
    }
}

void EnableDebugNotification(bool enable){
    sDebugNotificationEnabled = enable;
}

}
}
}
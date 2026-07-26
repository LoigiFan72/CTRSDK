// Filename: ro_ApiRead.cpp
//
// Project: Horizon

#include <cstring>
#include <nn/assert.h>
#include <nn/math.h>
#include <nn/util.h>
#include <nn/ro/ro_Result.h>
#include <nn/ro/ro_Info.h>
#include <nn/ro/ro_Api.h>
#include <nn/ro/ro_SectionTable.h>

namespace nn {
namespace ro {
namespace detail {
namespace{

typedef void (*StaticInitializer)();

// idea from NW4C :D
    inline s32 LookupPatricia(const PatriciaNode* root, const char* key){
        size_t len = std::strlen(key);

        const PatriciaNode* p;
        const PatriciaNode* x;
        s32 nextIndex;

        p = root;

        nextIndex = p->idxLeft;

        for(;;){
            x = root + (nextIndex & 0x7fff);

            if((nextIndex & 0x8000) != 0){
                break;
            }

            p = x;

            u32 wd  = x->ref >> 3;
            u32 pos = x->ref & 0x7;

            if (wd < len && ((key[wd] >> pos) & 0x1)){
                nextIndex = x->idxRight;
            }
            else{
            nextIndex = x->idxLeft;
            }
        }

        return x->value;
    }

    Result GetInitArrayRegion(CodeRegion* pRegion, void* pModule){
        return ControlObject(pModule, pRegion, OBJECT_CONTROL_GET_INIT_ARRAY);
    }

    Result GetStaticInitArrayRegion(CodeRegion* pRegion, void* pModule){
        return ControlObject(pModule, pRegion, OBJECT_CONTROL_GET_STATIC_INIT_ARRAY);
    }

    void CallInitializers(const CodeRegion* pRegion){
        StaticInitializer* pInitializer = reinterpret_cast<StaticInitializer*>(pRegion->begin);
        StaticInitializer* pInitializerEnd = reinterpret_cast<StaticInitializer*>(pRegion->end);

        for(; pInitializer < pInitializerEnd; ++pInitializer){
            (*pInitializer)();
        }
    }

    WorkArea* GetWorkAreaBuffer(void* pModule, void* pBuffer, size_t bufferSize){
        NN_UNUSED_VAR(bufferSize);

        const ModuleHeader& header = *reinterpret_cast<const ModuleHeader*>(pModule);
        WorkArea* pWork = reinterpret_cast<WorkArea*>(pBuffer);

        for(int i = 0; i < header.numSections; ++i){
            const SectionInfo& si = header.sectionInfo[i];

            if(si.section == SECTION_ZI){
                uptr bssEnd = si.offset.operator uptr() + si.size;
                pWork = reinterpret_cast<WorkArea*>(math::RoundUp(bssEnd, 4));

                break;
            }
            else if(si.section == SECTION_RW ){
                uptr dataEnd = si.offset.operator uptr() + si.size;
                pWork = reinterpret_cast<WorkArea*>(math::RoundUp(dataEnd, 4));

            }
        }

        return pWork;
    }

    WorkArea* GetWorkArea(void* pModule){
        WorkArea* pWork;
        Result result = ControlObject(pModule, &pWork, OBJECT_CONTROL_GET_WORK_AREA);
        return result.IsSuccess() ? pWork : NULL;
    }

    void SetWorkArea(void* pModule, WorkArea* pWorkBuffer){
        NN_DBG_CHECK_RESULT(ControlObject(pModule, pWorkBuffer, OBJECT_CONTROL_SET_WORK_AREA));
    }

} // namspace

bool GetAddress(uptr* pOut, const void* pModule, const char* symbolName){
    const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pModule);

    if (pHeader->numSymbolExportDictionaryNode == 0){
        return false;
    }

    s32 index = LookupPatricia(pHeader->symbolExportDictionary, symbolName);

    if((index < 0) || (index >= pHeader->numSymbolExports)){
        return false;
    }
    
    const SymbolExportTableEntry& entry = pHeader->symbolExportTable[index];

    if(std::strcmp(entry.symbol, symbolName) != 0){
        return false;
    }


    *pOut = detail::SectionTable(pHeader).GetAddress(entry.sectionAndOffset);

    return true;
}

bool GetAddressIn(uptr* pOut, const void* pFirst, const char* symbolName){
    for (const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pFirst); pHeader != NULL; pHeader = pHeader->node.pNext){
        if( GetAddress(pOut, pHeader, symbolName) ){
            return true;
        }
    }

    return false;
}

bool GetAddress(uptr* pOut, const void* pModule, int index){
    const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pModule);

    if (pHeader->numIndexExports < index || index < 0){
        return false;
    }

    const IndexExportTableEntry& entry = pHeader->indexExportTable[index];
    *pOut = detail::SectionTable(pHeader).GetAddress(entry.sectionAndOffset);

    return true;
}

bool GetBssSize(size_t* pBssSize, const void* pModule){
    const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pModule);

    NN_NULL_ASSERT_(pBssSize);

    *pBssSize = pHeader->bufferSize;

    return true;
}

void GetFixSizeInfo(SizeInfo* pInfo, const void* pModule){
    const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pModule);

#define UPDATE_MAX(v, h, a, s, u)    (v) = math::Max<uptr>((v), ((h)->a) + ((h)->s) * sizeof(u))
    uptr level3Offset = sizeof(*pHeader);
    UPDATE_MAX(level3Offset, pHeader, codeBinary,           codeBinarySize,         bit8);
    UPDATE_MAX(level3Offset, pHeader, baseStringTable,      baseStringTableSize,    bit8);
    UPDATE_MAX(level3Offset, pHeader, sectionInfo,          numSections,            SectionInfo);
    uptr level2Offset = level3Offset;
    UPDATE_MAX(level2Offset, pHeader, symbolExportTable,    numSymbolExports,       SymbolExportTableEntry);
    UPDATE_MAX(level2Offset, pHeader, indexExportTable,     numIndexExports,        IndexExportTableEntry);
    UPDATE_MAX(level2Offset, pHeader, exportStringTable,    exportStringTableSize,  bit8);
    UPDATE_MAX(level2Offset, pHeader, symbolExportDictionary, numSymbolExportDictionaryNode, SymbolExportTableEntry);
    uptr level1Offset = level2Offset;
    UPDATE_MAX(level1Offset, pHeader, referenceObjects,     numObjects,             SymbolExportTableEntry);
    UPDATE_MAX(level1Offset, pHeader, externalRelocation,   numExternalRelocations, ExternalRelocationTableEntry);
    UPDATE_MAX(level1Offset, pHeader, symbolImportTable,    numSymbolImports,       SymbolImportTableEntry);
    UPDATE_MAX(level1Offset, pHeader, indexImportTable,     numIndexImports,        IndexImportTableEntry);
    UPDATE_MAX(level1Offset, pHeader, offsetImportTable,    numOffsetImports,       OffsetImportTableEntry);
    UPDATE_MAX(level1Offset, pHeader, importStringTable,    importStringTableSize,  bit8);
    uptr level0Offset = level1Offset;
    UPDATE_MAX(level0Offset, pHeader, staticRelocation,     numStaticRelocations,   StaticRelocationTableEntry);
    UPDATE_MAX(level0Offset, pHeader, offsetExportTable,    numOffsetExports,       OffsetImportTableEntry);
    UPDATE_MAX(level0Offset, pHeader, internalRelocation,   numInternalRelocations, InternalRelocationTableEntry);
#undef UPDATE_MAX

    pInfo->mFix0End = level0Offset;
    pInfo->mFix1End = level1Offset;
    pInfo->mFix2End = level2Offset;
    pInfo->mFix3End = level3Offset;
    pInfo->mBufferSize = 0;
}

Result GetSizeInfo(SizeInfo* pInfo, const void* pModule){
    GetFixSizeInfo(pInfo, pModule);

    const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pModule);
    const uptr begin = reinterpret_cast<uptr>(pModule);

    const size_t bssWorkSize = pHeader->bufferSize;
    const size_t rwSize = pHeader->heapBinarySize;

    pInfo->mFix0End = begin + math::RoundUp(pInfo->mFix0End, 0x1000);
    pInfo->mFix1End = begin + math::RoundUp(pInfo->mFix1End, 0x1000);
    pInfo->mFix2End = begin + math::RoundUp(pInfo->mFix2End, 0x1000);
    pInfo->mFix3End = begin + math::RoundUp(pInfo->mFix3End, 0x1000);
    pInfo->mBufferSize = math::RoundUp(rwSize, 8) + bssWorkSize;

    return ResultSuccess();
}

bool GetName(const char** ppName, const void* pModule){
    const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pModule);

    *ppName = pHeader->moduleName;

    return true;
}

bool FindModule(void** pModule, void* pRoot, const char* moduleName){
    ModuleHeader* pRootHeader = reinterpret_cast<ModuleHeader*>(pRoot);

    for (ModuleHeader* pHeader = pRootHeader->node.pNext; pHeader != NULL; pHeader = pHeader->node.pNext){
        if (std::strcmp(moduleName,  pHeader->moduleName) == 0){
            *pModule = pHeader;
            return true;
        }
    }

    *pModule = NULL;
    return true;
}

bool InvokeProlog(void* pModule){
    ModuleHeader* pHeader = reinterpret_cast<ModuleHeader*>(pModule);

    {
        CodeRegion region;

        if(GetInitArrayRegion(&region, pHeader).IsSuccess()){
            CallInitializers(&region);
        }

        if(GetStaticInitArrayRegion(&region, pHeader).IsSuccess()){
            CallInitializers(&region);
        }
    }

    PrologFunction prolog = SectionTable(pHeader).GetPointer<PrologFunction>(pHeader->prolog);
    if(prolog != NULL){
        prolog();
    }

    return true;
}

bool InvokeEpilog(void* pModule){
    const ModuleHeader& header = *reinterpret_cast<ModuleHeader*>(pModule);

    WorkArea* pWork = GetWorkArea(pModule);
    NN_POINTER_TASSERT_(pWork);

    EpilogFunction epilog = SectionTable(&header).GetPointer<EpilogFunction>(header.epilog);
    if(epilog != NULL){
        epilog();
    }

    for(int i = pWork->numFinalizers - 1; i >= 0; --i){
        FinalizerEntry* pEntry = &pWork->pFinalizers[i];
        (pEntry->pFinalizer)(pEntry->pObject);
    }

    pWork->numFinalizers = 0;

    return true;
}

bool IsAllSymbolResolved(const void* pModule){
    const ModuleHeader* pHeader = reinterpret_cast<const ModuleHeader*>(pModule);
    bool prevIsLast = true;

    for (int i = 0 ; i < pHeader->numExternalRelocations ; ++i){
        const ExternalRelocationTableEntry& rel = pHeader->externalRelocation[i];

        if(prevIsLast){
            if (!rel.isResolved){
                return false;
            }
        }

        prevIsLast = rel.isLast;
    }

    return true;
}

void SetupWork(void* pModule, void* pBuffer, size_t bufferSize){
    WorkArea* pWork = GetWorkAreaBuffer(pModule, pBuffer, bufferSize);
    NN_POINTER_TASSERT_(pWork);
    SetWorkArea(pModule, pWork);

    FinalizerEntry* pFinalizerBuffer = reinterpret_cast<FinalizerEntry*>(pWork + 1);

    pWork->pFinalizers   = pFinalizerBuffer;
    pWork->numFinalizers = 0;
}

void GetRegionInfo(RegionInfo* pri, const void* pModule){
    const ModuleHeader& header = *reinterpret_cast<const ModuleHeader*>(pModule);

    uptr codeBegin = ~0u;
    uptr codeEnd = 0u;
    uptr bufBegin = ~0u;
    uptr bufEnd = 0u;

    for(s32 i = 0; i < header.numSections; ++i){
        const SectionInfo& si = header.sectionInfo[i];

        if(si.size == 0){
            continue;
        }

        switch(si.section){
        case SECTION_CODE:{
                codeBegin = math::Min<uptr>(codeBegin, si.offset);
                codeEnd = math::Max<uptr>(codeEnd, si.offset + si.size);
            }
            break;

        case SECTION_RW:{
                bufBegin = math::Min<uptr>(bufBegin, si.offset);
                bufEnd = math::Max<uptr>(bufEnd, si.offset + si.size);
            }
            break;

        case SECTION_ZI:{
                bufBegin = math::Min<uptr>(bufBegin, si.offset);
                bufEnd = math::Max<uptr>(bufEnd, si.offset + si.size);
            }
            break;

        default:
            break;
        }
    }

    pri->mMapBegin = reinterpret_cast<uptr>(pModule);
    pri->mMapSize = header.fixedSize;

    pri->mCroBegin = detail::GetOriginalAddress(pModule);
    pri->mCroSize = header.fixedSize;

    pri->mDataBssBegin = bufBegin;
    pri->mDataBssSize = bufEnd - bufBegin;

    pri->mCodeBegin = codeBegin;
    pri->mCodeSize = codeEnd - codeBegin;
}

Result ControlObject(void* pModule, void* param, ObjectControl c){
    const ModuleHeader& header = *reinterpret_cast<const ModuleHeader*>(pModule);
    SectionTable st(&header);

    ControlObjectFunc pFunc = st.GetPointer<ControlObjectFunc>(header.control);
    if(pFunc == NULL){
        return ResultControlObjectNotFound();
    }

    return pFunc(param, c);
}

}
}
}

extern "C" int nnroAeabiAtexit_(void* object, void (*destroyer)(void*), void* dso_handle){
    nn::ro::detail::WorkArea* pWork;
    void* pModule = nn::math::RoundDown(dso_handle, 0x1000);
    pWork= nn::ro::detail::GetWorkArea(pModule);
    NN_POINTER_TASSERT_(pWork);

    nn::ro::detail::FinalizerEntry* pEntry = &pWork->pFinalizers[pWork->numFinalizers++];

    pEntry->pObject = object;
    pEntry->pFinalizer = destroyer;

    return 0;
}
#pragma once

#include <nn/types.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/ro/ro_ObjectFile.h>
#include <nn/ro/ro_Types.h>

#define NN_RO_MAKE_SIGNATURE(a, b, c, d)  \
    ( (((a) & 0xFF) <<  0)          \
    | (((b) & 0xFF) <<  8)          \
    | (((c) & 0xFF) << 16)          \
    | (((d) & 0xFF) << 24) )

namespace nn { 
namespace ro { 
namespace detail {

const bit32 SIGNATURE_RO        = NN_RO_MAKE_SIGNATURE('C', 'R', 'O', '0');
const bit32 SIGNATURE_RO_FIXED  = NN_RO_MAKE_SIGNATURE('F', 'I', 'X', 'D');
const bit32 SIGNATURE_RR        = NN_RO_MAKE_SIGNATURE('C', 'R', 'R', '0');

struct CodeRegion{
    uptr begin;
    uptr end;
};

struct FinalizerEntry{
    void* pObject;
    void (*pFinalizer)(void*);
};

struct WorkArea{
    FinalizerEntry* pFinalizers;
    s32 numFinalizers;
    bit32 padding[6];
};

struct EitLinkNode{
    EitLinkNode* pPrev;
    EitLinkNode* pNext;
    uptr moduleBegin;
    uptr moduleEnd;
    bit64* pEitBegin;
    bit64* pEitEnd;
};

enum ObjectControl{
    OBJECT_CONTROL_GET_EIT_NODE,
    OBJECT_CONTROL_SET_WORK_AREA,
    OBJECT_CONTROL_GET_WORK_AREA,
    OBJECT_CONTROL_GET_INIT_ARRAY,
    OBJECT_CONTROL_GET_STATIC_INIT_ARRAY,

    OBJECT_CONTROL_MAX_BITS     = ~0u
};

typedef Result (*ControlObjectFunc)(void* param, ObjectControl c);

void SetupWork(void* pModule, void* pBuffer, size_t bufferSize);
Result GetSizeInfo(SizeInfo* pInfo, const void* pModule);
void GetFixSizeInfo(SizeInfo* pInfo, const void* pModule);
bool GetBssSize(size_t* pBssSize, const void* pModule);
bool GetName(const char** ppName, const void* pModule);
bool GetAddressIn(uptr* pOut, const void* pRoot, const char* symbolName);
bool GetAddress(uptr* pOut, const void* pModule, const char* symbolName);
bool GetAddress(uptr* pOut, const void* pModule, s32 index);
bool IsAllSymbolResolved(const void* pModule);
bool FindModule(void** ppModule, void* pRoot, const char* pModuleName);
bool GetVersion(u32* version, const void* pModule);
bool InvokeProlog(void* pModule);
bool InvokeEpilog(void* pModule);

Result ControlObject(void* pModule, void* param, ObjectControl objCtrlParam);

void GetRegionInfo(RegionInfo* pri, const void* pModule);

}
}
}
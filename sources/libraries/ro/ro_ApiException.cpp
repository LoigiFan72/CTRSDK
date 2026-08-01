// Filename: ro_ApiException.cpp
//
// Project: Horizon

#include <nn/srv.h>
#include <nn/svc.h>
#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/os.h>
#include <nn/os/ARM/os_MemoryBarrier.h>
#include <nn/math.h>
#include <nn/util/detail/util_Symbol.h>
#include <nn/ro.h>
#include <nn/ro/ro_DynamicLoader.h>
#include <nn/ro/ro_Info.h>

extern "C"{
    extern bit8 Load$$LR$$TEXT_SECTION$$Base[];
    extern bit8 Load$$LR$$TEXT_SECTION$$Limit[];
    extern __weak bit8 SHT$$ARM_EXIDX$$Base[];
    extern __weak bit8 SHT$$ARM_EXIDX$$Limit[];
}

namespace nn {
namespace ro {
namespace detail{
namespace{

struct EitSearchKey{
    uptr returnAddr;
    void* pEnd;
};

EitLinkNode sEitNode ={
    NULL,NULL,
    reinterpret_cast<uptr>(Load$$LR$$TEXT_SECTION$$Base),
    reinterpret_cast<uptr>(Load$$LR$$TEXT_SECTION$$Limit),
    reinterpret_cast<bit64*>(SHT$$ARM_EXIDX$$Base),
    reinterpret_cast<bit64*>(SHT$$ARM_EXIDX$$Limit)
};

EitLinkNode* sEitLinkHead = &sEitNode;
EitLinkNode* sEitLinkTail = &sEitNode;

void LinkEitNode(EitLinkNode* pNode){
    pNode->pPrev = sEitLinkTail;
    pNode->pNext = NULL;
    os::ARM::DataMemoryBarrier();

    sEitLinkTail->pNext = pNode;
    sEitLinkTail        = pNode;
}

void UnlinkEitNode(EitLinkNode* pNode){
    if(pNode == sEitLinkTail){
        sEitLinkTail = pNode->pPrev;
    }

    if(pNode->pNext != NULL){
        pNode->pNext->pPrev = pNode->pPrev;
    }

    pNode->pPrev->pNext = pNode->pNext;

    pNode->pNext = NULL;
    pNode->pPrev = NULL;
}

}

Result RegisterEit(Module* pModule){
    EitLinkNode* pEitNode = NULL;

    Result res = ControlObject(pModule, &pEitNode, OBJECT_CONTROL_GET_EIT_NODE);
    if(res.IsFailure() || (pEitNode == NULL) ){
        return ResultEitNodeNotFound();
    }

    ModuleHeader* pHeader = reinterpret_cast<ModuleHeader*>(pModule);
    pEitNode->moduleBegin = pHeader->codeBinary;
    pEitNode->moduleEnd   = pHeader->codeBinary + pHeader->codeBinarySize;

    LinkEitNode(pEitNode);

    return ResultSuccess();
}

Result UnregisterEit(Module* pModule){
    EitLinkNode* pEitNode = NULL;

    Result res = ControlObject(pModule, &pEitNode, OBJECT_CONTROL_GET_EIT_NODE);
    if(res.IsFailure() || (pEitNode == NULL)){
        return ResultEitNodeNotFound();
    }

    UnlinkEitNode(pEitNode);

    pEitNode->moduleBegin   = NULL;
    pEitNode->moduleEnd     = NULL;

    return ResultSuccess();
}

}
}
}
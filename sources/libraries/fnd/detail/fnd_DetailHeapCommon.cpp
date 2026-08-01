// Filename: fnd_DetailHeapCommon.cpp
//
// Project: Horizon

#include <nn/fnd/detail/fnd_DetailHeap.h>
#include <nn/fnd/detail/fnd_DetailList.h>
#include <nn/fnd/detail/fnd_DetailCommon.h>
#include <nn/fnd/detail/fnd_DetailHeapCommon.h>
#include <nn/fnd/detail/fnd_DetailHeapImpl.h>

namespace nn{
namespace fnd{
namespace detail{

static NNSFndList* FindListContainHeap(NNSiFndHeapHead* pHeapHd){
    NNSFndList* pList = &sRootList;

    NNSiFndHeapHead* pContainHeap = FindContainHeap(&sRootList, pHeapHd);

    if(pContainHeap){
        pList = &pContainHeap->childList;
    }

    return pList;
}

void NNSi_FndInitHeapHead(ExpHeapImpl* pHeapHd,u32 signature,void* heapStart,void* heapEnd,ushort optFlag){
    pHeapHd->signature = signature;

    pHeapHd->heapStart = heapStart;
    pHeapHd->heapEnd   = heapEnd;

    pHeapHd->attribute = 0;
    SetOptForHeap(pHeapHd, optFlag);
    FillNoUseMemory(pHeapHd,heapStart,GetOffsetFromPtr(heapStart, heapEnd));

    InitList(&pHeapHd->childList, 4);

    if (!sRootListInitialized){
        InitList(&sRootList, 4);
        sRootListInitialized = true;
    }

    NNSFndList* pRootList = &sRootList;
    NNSiFndHeapHead* pList = NULL;
    NNSFndList* pContain = &sRootList;

    AppendListObject(FindListContainHeap(pHeapHd), pHeapHd);
}

}
}
}
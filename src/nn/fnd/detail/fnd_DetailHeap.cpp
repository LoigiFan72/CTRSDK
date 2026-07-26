


#include <stdlib.h>
#include <nn/fnd/detail/fnd_DetailHeap.h>
#include <nn/fnd/detail/fnd_DetailList.h>
#include <nn/fnd/detail/fnd_DetailCommon.h>
#include <nn/fnd/detail/fnd_DetailHeapCommon.h>
#include <nn/fnd/detail/fnd_DetailHeapImpl.h>

#define MBLOCK_FREE_SIGNATURE   0x00004652 

#define MAX_ALIGNMENT           128

#define MIN_FREE_BLOCK_SIZE 4

namespace nn{
namespace fnd{
namespace detail{

typedef struct NNSiMemRegion NNSiMemRegion;

struct NNSiMemRegion{
    void* start;
    void* end;
};

/* Static Inlines n Shit */

static inline void* GetMemPtrForMBlock(NNSiFndExpHeapMBlockHead* pMBlkHd){
    return AddU32ToPtr(pMBlkHd, sizeof(NNSiFndExpHeapMBlockHead));
}

static inline void* GetMBlockEndAddr(NNSiFndExpHeapMBlockHead* pMBHead){
    return AddU32ToPtr(GetMemPtrForMBlock(pMBHead), pMBHead->blockSize);
}

static inline u16 GetAlignmentForMBlock(const NNSiFndExpHeapMBlockHead* pMBlkHd){
    return (u16)NNSi_FndGetBitValue(pMBlkHd->attribute, 8, 7);
}

static inline NNSiFndHeapHead* GetHeapHeadPtrFromHeapHead(NNSiFndExpHeapHead* pEHHead){
    return reinterpret_cast<NNSiFndHeapHead*>(SubU32ToPtr(pEHHead, sizeof(NNSiFndHeapHead)-sizeof(NNSiFndExpHeapHead)));
}

static inline u16 GetAllocMode(NNSiFndExpHeapHead* pEHHead){
    return (u16)NNSi_FndGetBitValue(pEHHead->feature, 0, 1);
}

static inline NNSiFndExpHeapHead * GetHeapHeadPtrFromHandle(Heap heap){
    return (NNSiFndExpHeapHead*)GetHeapHeadPtrFromHeapHead(heap);
}

static inline NNSiFndExpHeapHead* GetHeapHeadPtrFromHeapHead(NNSiFndHeapHead * pHHead){
    return &pHHead->nnsiFndExpHeapHead;
}

static inline NNSiFndExpHeapMBlockHead* GetMBlockHeadPtr(void* memBlock){
    return reinterpret_cast<NNSiFndExpHeapMBlockHead*>(SubU32ToPtr(memBlock, sizeof(NNSiFndExpHeapMBlockHead)));
}

static inline NNSiFndExpHeapHead const* GetHeapHeadPtrFromHeapHead(NNSiFndHeapHead const* pHHead){
    return &pHHead->nnsiFndExpHeapHead;
}

static inline void SetAllocDirForMBlock(NNSiFndExpHeapMBlockHead* pMBHead,u16 mode){
    NNSi_FndSetBitValue(pMBHead->attribute, 15, 1, mode);
}

static inline void SetAllocMode(NNSiFndExpHeapHead* pEHHead,u16 mode){
    NNSi_FndSetBitValue(pEHHead->feature, 0, 1, mode);
}

static inline void SetAlignmentForMBlock(NNSiFndExpHeapMBlockHead* pMBlkHd,u16 alignment){
    NNSi_FndSetBitValue(pMBlkHd->attribute, 8, 7, alignment);
}

static inline void SetGroupIDForMBlock(NNSiFndExpHeapMBlockHead* pMBHead,u16 id){
    NNSi_FndSetBitValue(pMBHead->attribute, 0, 8, id);
}

static void GetRegionOfMBlock(NNSiMemRegion* region,NNSiFndExpHeapMBlockHead* block){
    region->start = SubU32ToPtr(block, GetAlignmentForMBlock(block));
    region->end = GetMBlockEndAddr(block);
}

static NNSiFndExpHeapMBlockHead* InitMBlock(const NNSiMemRegion* pRegion,u16 signature){
    NNSiFndExpHeapMBlockHead* block = reinterpret_cast<NNSiFndExpHeapMBlockHead*>(pRegion->start);

    block->signature = signature;
    block->attribute = 0;
    block->blockSize = GetOffsetFromPtr(GetMemPtrForMBlock(block), pRegion->end);
    block->pMBHeadPrev = NULL;
    block->pMBHeadNext = NULL;

    return block;
}

static inline NNSiFndExpHeapMBlockHead* InitFreeMBlock(const NNSiMemRegion* pRegion){
    return InitMBlock(pRegion, MBLOCK_FREE_SIGNATURE);
}

static NNSiFndExpHeapMBlockHead* RemoveMBlock(NNSiFndExpMBlockList* list,NNSiFndExpHeapMBlockHead* block){
    NNSiFndExpHeapMBlockHead *const prev = block->pMBHeadPrev;
    NNSiFndExpHeapMBlockHead *const next = block->pMBHeadNext;

    if (prev){
        prev->pMBHeadNext = next;
    }
    else{
        list->head = next;
    }

    if (next){
        next->pMBHeadPrev = prev;
    }
    else{
        list->tail = prev;
    }

    return prev;
}

static NNSiFndExpHeapMBlockHead* InsertMBlock(NNSiFndExpMBlockList* list,NNSiFndExpHeapMBlockHead* target, NNSiFndExpHeapMBlockHead* prev){
    NNSiFndExpHeapMBlockHead* next;

    target->pMBHeadPrev = prev;
    if (prev){
        next = prev->pMBHeadNext;
        prev->pMBHeadNext = target;
    }
    else{
        next = list->head;
        list->head = target;
    }

    target->pMBHeadNext = next;
    if (next){
        next->pMBHeadPrev = target;
    }
    else{
        list->tail = target;
    }
    return target;
}

static inline void AppendMBlock(NNSiFndExpMBlockList* list,NNSiFndExpHeapMBlockHead* block){
    (void)InsertMBlock(list, block, list->tail);
}

static NNSiFndHeapHead* InitHeap(NNSiFndHeapHead* pHeapHead,void* startAddress,void* endAddress,u16 optFlag){
    NNSiFndHeapHead* pHeapHd = pHeapHead;
    NNSiFndExpHeapHead* pExpHeapHd = GetHeapHeadPtrFromHeapHead(pHeapHd);

    NNSi_FndInitHeapHead(pHeapHd,NNSI_EXPHEAP_SIGNATURE,startAddress,endAddress,optFlag);

    pExpHeapHd->groupID = 0;
    pExpHeapHd->feature = 0;
    SetAllocMode(pExpHeapHd, 0);

    {
        NNSiFndExpHeapMBlockHead* pMBHead;
        NNSiMemRegion region;
        region.start = pHeapHd->heapStart;
        region.end   = pHeapHd->heapEnd;
        pMBHead = InitFreeMBlock(&region);

        pExpHeapHd->mbFreeList.head = pMBHead;
        pExpHeapHd->mbFreeList.tail = pMBHead;
        pExpHeapHd->mbUsedList.head = NULL;
        pExpHeapHd->mbUsedList.tail = NULL;

        return pHeapHd;
    }
}

static bool RecycleRegion(NNSiFndExpHeapHead* pEHHead,const NNSiMemRegion* pRegion){
    NNSiFndExpHeapMBlockHead* pBlkPrFree  = NULL;
    NNSiMemRegion freeRgn = *pRegion;

    {
        NNSiFndExpHeapMBlockHead* pBlk;

        for (pBlk = pEHHead->mbFreeList.head; pBlk; pBlk = pBlk->pMBHeadNext){
            if (pBlk < pRegion->start){
                pBlkPrFree = pBlk;
                continue;
            }

            if (pBlk == pRegion->end){
                freeRgn.end = GetMBlockEndAddr(pBlk);
                (void)RemoveMBlock(&pEHHead->mbFreeList, pBlk);

                FillNoUseMemory(GetHeapHeadPtrFromHeapHead(pEHHead), pBlk, sizeof(NNSiFndExpHeapMBlockHead));
            }
            break;
        }
    }

    if (pBlkPrFree && GetMBlockEndAddr(pBlkPrFree) == pRegion->start){
        freeRgn.start = pBlkPrFree;
        pBlkPrFree = RemoveMBlock(&pEHHead->mbFreeList, pBlkPrFree);
    }

    if (GetOffsetFromPtr(freeRgn.start, freeRgn.end) < sizeof(NNSiFndExpHeapMBlockHead)){
        return false;
    }

    FillFreeMemory(GetHeapHeadPtrFromHeapHead(pEHHead), pRegion->start, GetOffsetFromPtr(pRegion->start, pRegion->end));

    (void)InsertMBlock(&pEHHead->mbFreeList,InitFreeMBlock(&freeRgn),pBlkPrFree);

    return true;
}

/* =========== */
/* =Functions= */
/* =========== */

ushort SetGroupIDForHelp(Heap heap, ushort groupId){
    ushort setGroupId;
    NNSiFndExpHeapHead* pEHHead;

    setGroupId = heap->groupID;
    heap->groupID = groupId;
    return setGroupId;
}

ushort SetAllocModeForHeap(Heap heap, ushort mode){
    ushort allocMode = heap->feature;
    heap->feature = mode & 1 | allocMode & 0xfffe;
    return allocMode & 1;
}

bool UseMarginOfAlignmentForHeap(Heap heap, bool reuse){
    bool isReuse = heap->reuse;
    heap->reuse = reuse;
    return isReuse;
}

/* Allocation */

static void* AllocUsedBlockFromFreeBlock(NNSiFndExpHeapHead* pEHHead, NNSiFndExpHeapMBlockHead* pMBHeadFree,void* mblock,u32 size,u16 direction){
    NNSiMemRegion freeRgnT;
    NNSiMemRegion freeRgnB;
    NNSiFndExpHeapMBlockHead* pMBHeadFreePrev;

    GetRegionOfMBlock(&freeRgnT, pMBHeadFree);
    freeRgnB.end   = freeRgnT.end;
    freeRgnB.start = AddU32ToPtr(mblock, size);
    freeRgnT.end   = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));

    pMBHeadFreePrev = RemoveMBlock(&pEHHead->mbFreeList, pMBHeadFree);

    if ((GetOffsetFromPtr(freeRgnT.start, freeRgnT.end) < sizeof(NNSiFndExpHeapMBlockHead) + MIN_FREE_BLOCK_SIZE) ||
       (direction == 0 && !pEHHead->reuse)){
        freeRgnT.end = freeRgnT.start;
    }
    else{
        pMBHeadFreePrev = InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnT), pMBHeadFreePrev);
    }

    if ((GetOffsetFromPtr(freeRgnB.start, freeRgnB.end) < sizeof(NNSiFndExpHeapMBlockHead) + MIN_FREE_BLOCK_SIZE) ||
       (direction == 1 && !pEHHead->reuse)){
        freeRgnB.start= freeRgnB.end;
    }
    else{
        (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnB), pMBHeadFreePrev);
    }

    FillAllocMemory(GetHeapHeadPtrFromHeapHead(pEHHead), freeRgnT.end, GetOffsetFromPtr(freeRgnT.end, freeRgnB.start));{
        NNSiFndExpHeapMBlockHead* pMBHeadNewUsed;
        NNSiMemRegion region;

        region.start = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));
        region.end   = freeRgnB.start;

        pMBHeadNewUsed = InitMBlock(&region, 0x5544);
        SetAllocDirForMBlock(pMBHeadNewUsed, direction);
        SetAlignmentForMBlock(pMBHeadNewUsed, (u16)GetOffsetFromPtr(freeRgnT.end, pMBHeadNewUsed));
        SetGroupIDForMBlock(pMBHeadNewUsed, pEHHead->groupID);
        AppendMBlock(&pEHHead->mbUsedList, pMBHeadNewUsed);
    }

    return mblock;
}

static void* AllocFromHead(NNSiFndHeapHead* pHeapHd,u32 size,int alignment){
    NNSiFndExpHeapHead* pExpHeapHd = GetHeapHeadPtrFromHeapHead(pHeapHd);

    const bool bAllocFirst = GetAllocMode(pExpHeapHd) == NN_OS_EXPHEAP_ALLOC_MODE_FIRST;

    NNSiFndExpHeapMBlockHead* pMBlkHd = NULL;
    NNSiFndExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xffffffff;
    void* foundMBlock = NULL;

    for (pMBlkHd = pExpHeapHd->mbFreeList.head; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadNext){
        void *const mblock    = GetMemPtrForMBlock(pMBlkHd);
        void *const reqMBlock = NNSi_FndRoundUpPtr(mblock, alignment);
        const u32 offset      = GetOffsetFromPtr(mblock, reqMBlock);

        if (pMBlkHd->blockSize >= size + offset &&  foundSize > pMBlkHd->blockSize){
            pMBlkHdFound  = pMBlkHd;
            foundSize     = pMBlkHd->blockSize;
            foundMBlock   = reqMBlock;

            if (bAllocFirst || foundSize == size){
                break;
            }
        }
    }

    if (!pMBlkHdFound){
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock(pExpHeapHd,pMBlkHdFound,foundMBlock,size,0);
}

static void* AllocFromTail(NNSiFndHeapHead* pHeapHd,u32 size,int alignment){
    NNSiFndExpHeapHead* pExpHeapHd = GetHeapHeadPtrFromHeapHead(pHeapHd);

    const bool bAllocFirst = GetAllocMode(pExpHeapHd) == NN_OS_EXPHEAP_ALLOC_MODE_FIRST;

    NNSiFndExpHeapMBlockHead* pMBlkHd      = NULL;
    NNSiFndExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xffffffff;
    void* foundMBlock = NULL;

    for (pMBlkHd = pExpHeapHd->mbFreeList.tail; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadPrev){
        void *const mblock    = GetMemPtrForMBlock(pMBlkHd);
        void *const mblockEnd = AddU32ToPtr(mblock, pMBlkHd->blockSize);
        void *const reqMBlock = NNSi_FndRoundDownPtr(SubU32ToPtr(mblockEnd, size), alignment);  // aligned address

        if (ComparePtr(reqMBlock, mblock) >= 0 &&  foundSize > pMBlkHd->blockSize ){
            pMBlkHdFound = pMBlkHd;
            foundSize    = pMBlkHd->blockSize;
            foundMBlock  = reqMBlock;

            if (bAllocFirst || foundSize == size){
                break;
            }
        }
    }

    if (!pMBlkHdFound){
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock(pExpHeapHd,pMBlkHdFound,foundMBlock,size,1);
}

#define MIN_ALIGNMENT 4

void* AllocFromHeap(Heap heap, size_t size, s32 alignment){
    void* memory = NULL;

    NN_TASSERT_(!(abs(alignment) & (abs(alignment) - 1)));
    NN_TASSERT_(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= MAX_ALIGNMENT);

    if (size == 0){
        size = 1;
    }

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);

    if (alignment >= 0){
        memory = AllocFromHead((ExpHeapImpl*)heap, size, alignment);
    }
    else{
        memory = AllocFromTail((ExpHeapImpl*)heap, size, -alignment);
    }

    return memory;
}

/* Freeing Heap */

void FreeToHeap(Heap heap,void* memBlock){
    {
        NNSiFndHeapHead* pHeapHd = (ExpHeapImpl*)heap;
        NNSiFndExpHeapHead* pExpHeapHd = GetHeapHeadPtrFromHandle((NNSiFndExpHeapHead*)pHeapHd);
        NNSiFndExpHeapMBlockHead* pMBHead = GetMBlockHeadPtr(memBlock);
        NNSiMemRegion region;

        NN_TASSERT_(pHeapHd->heapStart <= memBlock && memBlock < pHeapHd->heapEnd);

        GetRegionOfMBlock(&region, pMBHead);
        (void)RemoveMBlock(&pExpHeapHd->mbUsedList, pMBHead);
        (void)RecycleRegion(pExpHeapHd, &region);
    }
}

/* Creating Heap */

Heap CreateHeap(Heap heapHandle, void* startAddress, u32 size, ushort optFlag){
    void* endAddress;

    NN_NULL_TASSERT_(startAddress);

    endAddress   = NNSi_FndRoundDownPtr(AddU32ToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = NNSi_FndRoundUpPtr(startAddress, MIN_ALIGNMENT);

    if (NNSiGetUIntPtr(startAddress) > NNSiGetUIntPtr(endAddress) ||  GetOffsetFromPtr(startAddress, endAddress) < sizeof(NNSiFndExpHeapMBlockHead) + MIN_ALIGNMENT){
        return NULL;
    }

    {
        NNSiFndHeapHead* pHeapHd = InitHeap((NNSiFndHeapHead*)heapHandle, startAddress, endAddress, optFlag);
        return (Heap)pHeapHd;
    }
}

NNSiFndHeapHead* FindContainHeap(NNSFndList* pList, void* memBlock){
    NNSiFndHeapHead* pHeapHd = NULL;
    while (NULL != (pHeapHd = reinterpret_cast<NNSiFndHeapHead*>(
                    GetNextListObject(pList, pHeapHd)))){
        if(NNSiGetUIntPtr(pHeapHd->heapStart) <= NNSiGetUIntPtr(memBlock) &&  NNSiGetUIntPtr(memBlock) < NNSiGetUIntPtr(pHeapHd->heapEnd)){
            NNSiFndHeapHead* pChildHeapHd = FindContainHeap(&pHeapHd->childList, memBlock);
            if(pChildHeapHd){
                return pChildHeapHd;
            }
            return pHeapHd;
        }
    }
    return 0;
}

}
}
}
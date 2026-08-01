// Filename: fnd_DetailList.cpp
//
// Project: Horizon

#include <nn/fnd/detail/fnd_DetailList.h>
#include <nn/fnd/detail/fnd_DetailHeapHead.h>
#include <nn/fnd/detail/fnd_DetailHeap.h>
#pragma PRESERVE8 // If using ARMCC

namespace nn{
namespace fnd{
namespace detail{

void RemoveListObject(NNSFndList* list, void* object) {
    NNSFndLink* link = (NNSFndLink*)((u8*)object + list->offset);

    if (link->prevObject == 0) {
        list->headObject = link->nextObject;
    } 
    else{
        ((NNSFndLink*)((u8*)link->prevObject + list->offset))->nextObject = link->nextObject;
    }

    if (link->nextObject == 0) {
        list->tailObject = link->prevObject;
    } 
    else{
        ((NNSFndLink*)((u8*)link->nextObject + list->offset))->prevObject = link->prevObject;
    }

    link->prevObject = 0;
    link->nextObject = 0;
    list->numObjects--;
}

void AppendListObject(NNSFndList* list, void* object) {
    if (list->headObject == 0){
        NNSFndLink* link = (NNSFndLink*)((u8*)object + list->offset);
        link->nextObject = 0;
        link->prevObject = 0;
        list->headObject = object;
        list->tailObject = object;
        list->numObjects++;
    }
    else{
        NNSFndLink* link = (NNSFndLink*)((u8*)object + list->offset);
        link->prevObject = list->tailObject;
        link->nextObject = 0;
        ((NNSFndLink*)((u8*)list->tailObject + list->offset))->nextObject = object;
        list->tailObject = object;
        list->numObjects++;
    }
}

void* GetNextListObject(NNSFndList* list, void* object) {
    if (object == 0) {
        return list->headObject;
    }
    return ((NNSFndLink*)((u8*)object + list->offset))->nextObject;
}

void InitList(NNSFndList* list, ushort offset){
    list->headObject = 0;
    list->tailObject = 0;
    list->numObjects = 0;
    list->offset = offset;
}

}
}
}
#pragma once

#include <nn/types.h>

namespace nn{ 
namespace fnd{ 
namespace detail{

    struct NNSFndLink
    {
        void* prevObject;
        void* nextObject;
    };

    struct NNSFndList
    {
        void* headObject;
        void* tailObject;
        u16 numObjects;
        u16 offset;
    };

    struct NNSiFndExpHeapMBlockHead 
    {
        u16 signature;
        u16 attribute;
        u32 blockSize;
        NNSiFndExpHeapMBlockHead* pMBHeadPrev;
        NNSiFndExpHeapMBlockHead* pMBHeadNext;
    };

    struct NNSiFndExpMBlockList
    {
        NNSiFndExpHeapMBlockHead* head;
        NNSiFndExpHeapMBlockHead* tail;
    };

    struct NNSiFndExpHeapHead
    {
        NNSiFndExpMBlockList mbFreeList;
        NNSiFndExpMBlockList mbUsedList;
        u16 groupID;
        u16 feature;
        bool reuse;
        u8 padding[3];
    };

    struct ExpHeapImpl
    {
        ExpHeapImpl() {}
        
        u32 signature;
        NNSFndLink link;
        NNSFndList childList;
        void* heapStart;
        void* heapEnd;
        u32 attribute;
        NNSiFndExpHeapHead nnsiFndExpHeapHead;
    };

    typedef ExpHeapImpl NNSiFndHeapHead;

}
}
}
#pragma once

#include <nn/os/os_Event.h>
#include <nn/util/util_SizedEnum.h>

namespace nn{
namespace gxlow{
namespace CTR{
namespace detail{
struct CfReq {
    bit32 addr0;
    bit32 size0;
    bit32 addr1;
    bit32 size1;
    bit32 addr2;
    bit32 size2;
    bit32 rsv6;
};

struct PpfReq {
    bit32 srcAddr;
    bit32 dstAddr;
    bit32 srcSize;
    bit32 dstSize;
    bit32 mode;
    bit32 rsv5;
    bit32 rsv6;
};

struct P3dReq {
    bit32 addr;
    bit32 size;
    bit32 control;
    bit32 rsv3;
    bit32 rsv4;
    bit32 rsv5;
    bit32 cacheFlush;
};

struct PpfTcReq {
    bit32 srcAddr;
    bit32 dstAddr;
    bit32 dmaSize;
    bit32 srcInterval;
    bit32 dstInterval;
    bit32 mode;
    bit32 rsv6;
};

struct DmaReq {
    bit32 srcAddr;
    bit32 dstAddr;
    bit32 size;
    bit32 rsv3;
    bit32 rsv4;
    bit32 rsv5;
    bit32 cacheFlush;
};

struct FillReq {
    bit32 start0;
    bit32 data0;
    bit32 end0;
    bit32 start1;
    bit32 data1;
    bit32 end1;
    bit16 ctrl0;
    bit16 ctrl1;
};

enum CmdReqId{
    REQ_ID_DMA,
    REQ_ID_3D_CMD,
    REQ_ID_MEM_FILL,
    REQ_ID_DISP_COPY,
    REQ_ID_DISP_COPY_2,
    REQ_ID_CACHE_FLUSH
};


struct CmdReq {
    util::SizedEnum1<CmdReqId> id;
    bool callbackEnabled;
    bool stopEnabled;
    bool sync;
    union CmdReqParam {
        union{
            bit32 d[7];
        } data;
        DmaReq dma;
        P3dReq ren;
        FillReq mf;
        PpfReq pf;
        PpfTcReq ctx;
        CfReq cf;
    } param;
};

class CmdReqQueueBase{
protected:
    static const s32 QUEUE_LENGTH  = 15;
    
    struct QueueControl {
        u8 head;
        u8 usedCount;
        bit8 status;
        bit8 control;
    };

    union QueueControlPacker{
        QueueControl qc;
        bit32 packed32;
    };

    struct QueueBody {
        QueueControl control;
        Result lastResult;
        bit32 pad[6];
        CmdReq data[15];
    };

    QueueBody* mpBody;

    CmdReqQueueBase(): 
        mpBody(0) 
    {}
    ~CmdReqQueueBase() {}
public:
    void Initialize(void* pQueueBody);
    void Finalize();
};

inline void CmdReqQueueBase::Initialize(void* pQueueBody){
    NN_TASSERT_(pQueueBody != 0);
    mpBody = reinterpret_cast<QueueBody*>(pQueueBody);
}

inline void CmdReqQueueBase::Finalize(){
    mpBody = NULL;
}

}

class CmdReqQueueTx : public detail::CmdReqQueueBase{
public:
    void Initialize(void* pQueueBody);
    void Finalize();
    Result TryEnqueue(const detail::CmdReq* pCmdReq);
private:
    void Reset();
};

}
}
}
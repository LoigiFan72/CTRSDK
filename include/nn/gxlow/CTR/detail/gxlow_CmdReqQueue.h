#pragma once

#include <nn/os/os_Event.h>
#include <nn/util/util_SizedEnum.h>

namespace nn{
namespace gxlow{
namespace CTR{
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

union CmdReqParam {
    union cmdData {
        bit32 d[7];
    };
    cmdData data;
    DmaReq dma;
    P3dReq ren;
    FillReq mf;
    PpfReq pf;
    PpfTcReq ctx;
    CfReq cf;
};

struct CmdReq {
    util::SizedEnum1<CmdReqId> id;
    bool callbackEnabled;
    bool stopEnabled;
    bool sync;
    union CmdReqParam param;
};

class CmdReqQueueBase{
public:
    struct QueueControl {
        u8 head;
        u8 usedCount;
        bit8 status;
        bit8 control;
    };

    struct QueueBody {
        struct QueueControl control;
        struct Result lastResult;
        bit32 pad[6];
        struct CmdReq data[15];
    };

    QueueBody* mpBody;

    void Initialize(void* pQueueBody){
        NN_TASSERT_(pQueueBody != 0);
        this->mpBody = reinterpret_cast<QueueBody*>(pQueueBody);
    }

    void Finalize(){
        this->mpBody = 0;
    }
};

class CmdReqQueueTx : public CmdReqQueueBase{
public:
    void Initialize(void*);
    void Finalize();
    Result TryEnqueue(CmdReq* pCmdReq);
};

/* DisplaySwap */ // MOVE TO gxlow_DisplaySwapInfoPad.h

struct DisplaySwapInfo {
    s32 nextBank;
    uptr address;
    uptr addressB;
    size_t size;
    bit32 mode;
    bit32 swap;
    bit32 attribute;
};

class DisplaySwapInfoBase{
    struct PadControl {
        u8 currentIndex;
        bool update;
        bit8 pad[2];
    };

    struct PadBody {
        struct PadControl control;
        struct DisplaySwapInfo info[2];
        bit32 reserved;
    };
    PadBody* mpBody[2];
public:
    void Initialize(void* pPadBody){
        NN_TASSERT_(!pPadBody);
        this->mpBody[0] = (PadBody*) pPadBody;
        for(int i = 1; i < 2; i++){
            this->mpBody[i] = this->mpBody[i + -1];
            this->mpBody[i] = this->mpBody[i] + 1;
        }
    }
    void Finalize(){
        for(int i = 0; i < 2; i++){
            this->mpBody[i] = 0;
        }
    }
};

class DisplaySwapInfoTx : public DisplaySwapInfoBase{

};

}
}
}
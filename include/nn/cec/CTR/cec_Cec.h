#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>

namespace nn {
namespace cec {
namespace CTR {
namespace detail {

class Cec{
public:
    static Handle s_Session;
    Cec() {}

public:
    static Result Open(u32 cecTitleId, u32 dataType, u32 option, size_t* filesize);
    static Result Read(size_t* pReadLen, u8 pReadBuf[], size_t len );
    static Result ReadMessage(u32 cecTitleId, u8 in_or_out_box, const u8 pMessId[], size_t messIdLen, size_t* pReadLen, u8 pReadBuf[], size_t len);
    static Result Write(const u8 pWriteBuf[], size_t len );
    static Result WriteMessageWithHmac(u32 cecTitleId, u8 in_or_out_box, u8 pMessId[], size_t messIdLen, const u8 pWriteBuf[], size_t len, const u8 pHmacKey[]);
    static Result Delete(u32 cecTitleId, u32 dataType, u8 in_or_out_box, const u8 pMessId[], size_t messIdLen);
    static Result SetData(u32 cecTitleId, const u8 pData[], size_t len, u32 option);
    static Result ReadData(u8 pReadBuf[], size_t len, u32 option, const u8 optionData[], size_t optionDataLen);
    static Result Start(u32 option);
    static Result Stop(u32 option);
    static Result GetCecdState(u32* state);
    static Result GetChangeStateEventHandle(Handle* pEventHandle);
};

}
}
}
}
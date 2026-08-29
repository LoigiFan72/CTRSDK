#pragma once

#include <nn/cec/CTR/cec_Cec.h>

namespace nn{
namespace cec{
namespace CTR{
namespace detail{
namespace{
    const char PORT_NAME_CEC[]      = "cecd:u";
    const char PORT_NAME_CEC_SYS[]  = "cecd:s";
}

Result InitializeCecControl();
Result InitializeCecControlSys();
Result FinalizeCecControl();
Result FinalizeCecControlSys();

Result WaitForSessionValid();


Result Open(u32 cecTitleId, u32 dataType, u32 option, size_t* filesize);
Result Read(size_t* pReadLen, u8 pReadBuf[], size_t len);
Result ReadMessage(u32 cecTitleId, u8 in_or_out_box, const u8 pMessId[], size_t messIdLen, size_t* pReadLen, u8 pReadBuf[], size_t len);
Result ReadMessageWithHmac(u32 cecTitleId, u8 in_or_out_box, const u8 pMessId[], size_t messIdLen, size_t* pReadLen, u8 pReadBuf[], size_t len , const u8 pHmac[]);
Result Write(const u8 pWriteBuf[], size_t len );
Result WriteMessage(u32 cecTitleId, u8 in_or_out_box, u8 pMessId[], size_t messIdLen, const u8 pWriteBuf[], size_t len );
Result WriteMessageWithHmac(u32 cecTitleId, u8 in_or_out_box, u8 pMessId[], size_t messIdLen, const u8 pWriteBuf[], size_t len , const u8 pHmac[]);
Result Delete(u32 cecTitleId, u32 dataType, u8 in_or_out_box, const u8 pMessId[], size_t messIdLen);

Result SetData(u32 cecTitleId, const u8 pData[], size_t len, u32 option);
Result ReadData(u8 pReadBuf[], size_t len, u32 option , const u8 optionData[], size_t optionDataLen);

Result Start(u32 option);
Result Stop(u32 option);

Result GetCecdState(u32* state);
Result GetChangeStateEventHandle(Handle* pEventHandle);

Result SetCecTestMode(bit8 testMode);
Result GetCecInfoEventHandleSys(Handle* pEventHandle);

}
}
}
}
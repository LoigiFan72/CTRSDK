#pragma once

#include <nn/Handle.h>

namespace nn {
namespace dsp {
namespace CTR {
class DSP{
public:
    Handle m_Session;
 
    DSP(){ }
    DSP(Handle h){ this->m_Session = h; }
    Result ConvertProcessAddressFromDspDram(uptr _address,uptr *address);
    Result FlushDataCache(Handle clientProcess,uptr addr,size_t size);
    Result GetSemaphoreEventHandle(Handle *handle);
    Result LoadComponent(const u8 *pComponent,size_t size,bit16 maskPram,bit16 maskDram,bool *pStatus);
    Result ReadPipeIfPossible(s32 port,s32 peer,u8 *pBuffer,u16 length,u16 *lengthRead);
    Result RecvData(u16 regNo, u16* pValue);
    Result RecvDataIsReady(u16 regNo,bool *pReady);
    Result RegisterInterruptEvents(Handle handle,s32 type,s32 port);
    Result SetSemaphore(u16 mask);
    Result SetSemaphoreMask(u16 mask);
    Result UnloadComponent();
    Result WriteProcessPipe(s32 port,const u8 *pBuffer,size_t length);
};
}
}
}
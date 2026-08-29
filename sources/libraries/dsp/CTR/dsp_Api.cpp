// Filename: dsp_Api.cpp
//
// Project: Horizon

#include <nn/dsp/CTR/MPCore/dsp_Api.h>
#include <nn/dsp/CTR/dsp_Result.h>
#include <nn/srv/srv_API.h>
#include <nn/applet/CTR/applet_Wrapper.h>
#include <nn/err/CTR/err_Api.h>

#include <string.h>

extern const u8 DSPSND_BEGIN[]={0}; // place the holder

const int CALLBACK_NUM = 8;

namespace nn{
namespace dsp{
namespace CTR{
namespace{
    DSP* s_pDspSession;
    u8 s_DspSessionObject[sizeof(nn::dsp::CTR::DSP)];
    Handle s_DspSessionHandle;
    bool s_IsSleepAcceptedCallbackCalled;
    bool s_IsComponentLoaded;
    bool s_IsSleeping;
    int s_DspEventUsedFlag;

    void (*s_SleepCallback[CALLBACK_NUM])(void);
    void (*s_WakeUpCallback[CALLBACK_NUM])(void);
    void (*s_FinalizeCallback[CALLBACK_NUM])(void);
    const u8* s_RegisteredComponent;
    size_t s_RegisteredComponentSize;
    ushort s_RegisteredProgMask;
    ushort s_RegisteredDataMask;

    applet::CTR::SysSleepAcceptedCallbackInfo s_SleepAcceptedCallbackInfo;
}

Result Initialize(){
    if (s_pDspSession != NULL)
        return ResultSuccess();

    NN_UTIL_RETURN_IF_FAILED(srv::Initialize());

    NN_UTIL_RETURN_IF_FAILED(InitializeIpc(&s_DspSessionHandle));

    s_pDspSession = reinterpret_cast<DSP*>(s_DspSessionObject);

    *s_pDspSession = DSP(s_DspSessionHandle);

    for (int i = 0; i < CALLBACK_NUM; i++){
        s_SleepCallback[i] = s_WakeUpCallback[i] = s_FinalizeCallback[i] = NULL;
    }

    s_IsComponentLoaded = false;
    s_RegisteredComponent = NULL;
    s_RegisteredComponentSize = 0;
    s_RegisteredProgMask = 0;
    s_RegisteredDataMask = 0;
    s_IsSleeping = false;

    s_SleepAcceptedCallbackInfo.Register();

    return ResultSuccess();
}

void Finalize(){
    if (s_pDspSession){
        if (s_DspEventUsedFlag != 0){
            NN_TPANIC_("Interrupt events are still registered.");
        }

        for (int i = 0; i < CALLBACK_NUM; i++){
            if (s_SleepCallback[i] != NULL){
                NN_TPANIC_("Callbacks are still registered.");
            }
        }

        s_SleepAcceptedCallbackInfo.Unregister();

        nn::dsp::CTR::FinalizeIpc(&s_DspSessionHandle);
        s_DspSessionHandle = INVALID_HANDLE_VALUE;
        s_pDspSession = NULL;
    }
}

Result LoadDefaultComponent(){
    return LoadComponent(DSPSND_BEGIN,0xc234, 0xff, 0xff);
}

Result LoadComponent(const u8* pComponent, size_t size, bit16 maskPram, bit16 maskDram){
    s_RegisteredComponent = pComponent;
    s_RegisteredComponentSize = size;
    s_RegisteredProgMask = maskPram;
    s_RegisteredDataMask = maskDram;
    return LoadComponentCore(pComponent, size, maskPram, maskDram);
}

Result LoadComponentCore(const u8* pComponent, size_t size, bit16 maskPram, bit16 maskDram){
    Result res;

    if(s_pDspSession == 0 && (!s_IsComponentLoaded)){
        return s_pDspSession->LoadComponent(pComponent,size,maskPram,maskDram,&s_IsComponentLoaded);
    }

    else{
        return ResultAlreadyExists();
    }
    return res;
}

Result UnloadComponent(){
    return UnloadComponentCore();
}

Result UnloadComponentCore(){
    Result res = ResultSuccess();
    if(s_IsComponentLoaded){
        res = s_pDspSession->UnloadComponent();
        s_IsComponentLoaded = false;
    }
    return res;
}

Result RegisterInterruptEvents(nn::Handle handle, s32 type, s32 port){
    Result res = ResultNotInitialized();
    if (s_pDspSession){
        if (handle.IsValid() && (s_DspEventUsedFlag & (0x1 << (type + port))) == 0){
            res = s_pDspSession->RegisterInterruptEvents(handle, type, port);
            s_DspEventUsedFlag |=  (0x1 << (type + port));
        }

        if (!handle.IsValid() && (s_DspEventUsedFlag & (0x1 << (type + port))) != 0){
            res = s_pDspSession->RegisterInterruptEvents(handle, type, port);
            s_DspEventUsedFlag &= ~(0x1 << (type + port));
        }
    }
    return res;
}

Result RecvData(u16 regNo, u16* pValue){
    Result res = ResultNotInitialized();
    if(s_pDspSession){
        res = s_pDspSession->RecvData(regNo,pValue);
    }
    return res;
}

Result RecvDataIsReady(u16 regNo, bool* pStatus){
    Result res = ResultNotInitialized();
    if(s_pDspSession){
        res = s_pDspSession->RecvDataIsReady(regNo,pStatus);
    }
    return res;
}

Result ConvertProcessAddressFromDspDram(uptr addressOnDsp, uptr* pAddressOnHost){
    Result res = ResultNotInitialized();
    *pAddressOnHost = 0xffffffff;
    if(s_pDspSession){
        res = s_pDspSession->ConvertProcessAddressFromDspDram(addressOnDsp,pAddressOnHost);
    }
    return res;
}

Result ReadPipeIfPossible(int port, void* buffer, u16 length, u16* pLengthRead){
    NN_NULL_TASSERT_(buffer);
    NN_NULL_TASSERT_(pLengthRead);
    Result res = ResultNotInitialized();
    if (s_pDspSession){
        res = s_pDspSession->ReadPipeIfPossible(port, 0, (u8 *)buffer, length, pLengthRead);
    }
    else{
        *pLengthRead = NULL;
    }
    return res;
}

Result WriteProcessPipe(int port, const void* buffer, u32 length){
    Result res = ResultNotInitialized();
    if(s_pDspSession){
        res = s_pDspSession->WriteProcessPipe(port,(u8*)buffer,length);
    }
    return res;
}

Result FlushDataCache(uptr addr, size_t size){
    Result res = ResultNotInitialized();
    if(s_pDspSession){
        Handle h;
        res = s_pDspSession->FlushDataCache(h,addr,size);
    }
    return res;
}

bool IsComponentLoaded(){
    return s_IsComponentLoaded;
}

bool Sleep(){
    if (IsComponentLoaded() && s_IsSleeping == false){
        for (int i = 0; i < CALLBACK_NUM; i++){
            if (s_SleepCallback[i]) s_SleepCallback[i]();
        }

        UnloadComponentCore();
        s_IsSleeping = true;
        return true;
    }

    else{
        return false;
    }
}

void WakeUp(){
    s_IsSleepAcceptedCallbackCalled = 0;
    if(s_IsSleeping){
        NN_TASSERT_(sRegisteredComponent != NULL);
        NN_ERR_THROW_FATAL(LoadComponentCore(s_RegisteredComponent,s_RegisteredComponentSize,s_RegisteredProgMask,s_RegisteredDataMask));
        for (int i = 0; i < CALLBACK_NUM; i++){
            if (s_WakeUpCallback[i]) s_WakeUpCallback[i]();
        }
        s_IsSleeping = false;
    }
}

void Awake(){
    if(s_IsSleepAcceptedCallbackCalled) 
        WakeUp();
}

void OrderToWaitForFinalize(){
    if(s_IsSleeping){
        for(int i = 0; i < CALLBACK_NUM; i++){
            if (s_FinalizeCallback[i]) s_FinalizeCallback[i]();
        }
        s_IsSleeping = false;
    }
}

bool RegisterSleepWakeUpCallback(void (*sleepCallback)(),void (*wakeUpCallback)(),void (*finalizeCallback)()){
    for (int i = 0; i < CALLBACK_NUM; i++){
        if (s_SleepCallback[i] == NULL){
            NN_TASSERT_(s_WakeUpCallback[i] == NULL);
            NN_TASSERT_(s_FinalizeCallback[i] == NULL);
            s_SleepCallback[i] = sleepCallback;
            s_WakeUpCallback[i] = wakeUpCallback;
            s_FinalizeCallback[i] = finalizeCallback;
            return true;
        }
    }
}

bool ClearSleepWakeUpCallback(void (*sleepCallback)(),void (*wakeUpCallback)(),void (*finalizeCallback)()){
    for (int i = 0; i < CALLBACK_NUM; i++){
        if (s_SleepCallback[i] == sleepCallback){
            NN_TASSERT_(s_WakeUpCallback[i] == wakeUpCallback);
            NN_TASSERT_(s_FinalizeCallback[i] == finalizeCallback);
            s_SleepCallback[i] = NULL;
            s_WakeUpCallback[i] = NULL;
            s_FinalizeCallback[i] = NULL;
            return true;
        }
    }
    return false;
}

}
}
}
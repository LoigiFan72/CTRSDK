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
    static DSP* spDspSession;
    u8 sDspSessionObject[sizeof(nn::dsp::CTR::DSP)];
    static Handle sDspSessionHandle;
    static bool sIsSleepAcceptedCallbackCalled;
    static bool sIsComponentLoaded;
    static bool sIsSleeping;
    static int sDspEventUsedFlag;

    void (*sSleepCallback[CALLBACK_NUM])(void);
    void (*sWakeUpCallback[CALLBACK_NUM])(void);
    void (*sFinalizeCallback[CALLBACK_NUM])(void);
    static const u8* sRegisteredComponent;
    static size_t sRegisteredComponentSize;
    static ushort sRegisteredProgMask;
    static ushort sRegisteredDataMask;

    static applet::CTR::SysSleepAcceptedCallbackInfo sSleepAcceptedCallbackInfo;
}

Result Initialize(){
    if (spDspSession != NULL)
        return ResultSuccess();

    NN_UTIL_RETURN_IF_FAILED(srv::Initialize());

    NN_UTIL_RETURN_IF_FAILED(InitializeIpc(&sDspSessionHandle));

    spDspSession = reinterpret_cast<DSP*>(sDspSessionObject);

    *spDspSession = DSP(sDspSessionHandle);

    for (int i = 0; i < CALLBACK_NUM; i++){
        sSleepCallback[i] = sWakeUpCallback[i] = sFinalizeCallback[i] = NULL;
    }

    sIsComponentLoaded = false;
    sRegisteredComponent = NULL;
    sRegisteredComponentSize = 0;
    sRegisteredProgMask = 0;
    sRegisteredDataMask = 0;
    sIsSleeping = false;

    sSleepAcceptedCallbackInfo.Register();

    return ResultSuccess();
}

void Finalize(){
    if (spDspSession){
        if (sDspEventUsedFlag != 0){
            NN_TPANIC_("Interrupt events are still registered.");
        }

        for (int i = 0; i < CALLBACK_NUM; i++){
            if (sSleepCallback[i] != NULL){
                NN_TPANIC_("Callbacks are still registered.");
            }
        }

        sSleepAcceptedCallbackInfo.Unregister();

        nn::dsp::CTR::FinalizeIpc(&sDspSessionHandle);
        sDspSessionHandle = INVALID_HANDLE_VALUE;
        spDspSession = NULL;
    }
}

Result LoadDefaultComponent(){
    return LoadComponent(DSPSND_BEGIN,0xc234, 0xff, 0xff);
}

inline Result LoadComponent(const u8* pComponent, size_t size, bit16 maskPram, bit16 maskDram){
    sRegisteredComponent = pComponent;
    sRegisteredComponentSize = size;
    sRegisteredProgMask = maskPram;
    sRegisteredDataMask = maskDram;
    return LoadComponentCore(pComponent, size, maskPram, maskDram);
}

inline Result LoadComponentCore(const u8* pComponent, size_t size, bit16 maskPram, bit16 maskDram){
    Result res;
    if(spDspSession == 0 && (!sIsComponentLoaded)){
        return spDspSession->LoadComponent(pComponent,size,maskPram,maskDram,&sIsComponentLoaded);
    }
    else{
        return ResultAlreadyExists();
    }
    return res;
}

Result UnloadComponent(){
    return UnloadComponentCore();
}

inline Result UnloadComponentCore(){
    Result res = ResultSuccess();
    if(sIsComponentLoaded){
        res = spDspSession->UnloadComponent();
        sIsComponentLoaded = false;
    }
    return res;
}

Result RegisterInterruptEvents(nn::Handle handle, s32 type, s32 port){
    Result res = ResultNotInitialized();
    if (spDspSession){
        if (handle.IsValid() && (sDspEventUsedFlag & (0x1 << (type + port))) == 0){
            res = spDspSession->RegisterInterruptEvents(handle, type, port);
            sDspEventUsedFlag |=  (0x1 << (type + port));
        }
        if (!handle.IsValid() && (sDspEventUsedFlag & (0x1 << (type + port))) != 0){
            res = spDspSession->RegisterInterruptEvents(handle, type, port);
            sDspEventUsedFlag &= ~(0x1 << (type + port));
        }
    }
    return res;
}

Result RecvData(u16 regNo, u16* pValue){
    Result res = ResultNotInitialized();
    if(spDspSession){
        res = spDspSession->RecvData(regNo,pValue);
    }
    return res;
}

Result RecvDataIsReady(u16 regNo, bool* pStatus){
    Result res = ResultNotInitialized();
    if(spDspSession){
        res = spDspSession->RecvDataIsReady(regNo,pStatus);
    }
    return res;
}

Result ConvertProcessAddressFromDspDram(uptr addressOnDsp, uptr* pAddressOnHost){
    Result res = ResultNotInitialized();
    *pAddressOnHost = 0xffffffff;
    if(spDspSession){
        res = spDspSession->ConvertProcessAddressFromDspDram(addressOnDsp,pAddressOnHost);
    }
    return res;
}

Result ReadPipeIfPossible(int port, void* buffer, u16 length, u16* pLengthRead){
    NN_NULL_TASSERT_(buffer);
    NN_NULL_TASSERT_(pLengthRead);
    Result res = ResultNotInitialized();
    if (spDspSession){
        res = spDspSession->ReadPipeIfPossible(port, 0, (u8 *)buffer, length, pLengthRead);
    }
    else{
        *pLengthRead = NULL;
    }
    return res;
}

Result WriteProcessPipe(int port, const void* buffer, u32 length){
    Result res = ResultNotInitialized();
    if(spDspSession){
        res = spDspSession->WriteProcessPipe(port,(u8*)buffer,length);
    }
    return res;
}

Result FlushDataCache(uptr addr, size_t size){
    Result res = ResultNotInitialized();
    if(spDspSession){
        Handle h;
        res = spDspSession->FlushDataCache(h,addr,size);
    }
    return res;
}

bool IsComponentLoaded(){
    return sIsComponentLoaded;
}

bool Sleep(){
    if (IsComponentLoaded() && sIsSleeping == false){
        for (int i = 0; i < CALLBACK_NUM; i++){
            if (sSleepCallback[i]) sSleepCallback[i]();
        }
        UnloadComponentCore();
        sIsSleeping = true;
        return true;
    }
    else{
        return false;
    }
}

void WakeUp(){
    sIsSleepAcceptedCallbackCalled = 0;
    if(sIsSleeping){
        NN_TASSERT_(sRegisteredComponent != NULL);
        NN_ERR_THROW_FATAL(LoadComponentCore(sRegisteredComponent,sRegisteredComponentSize,sRegisteredProgMask,sRegisteredDataMask));
        for (int i = 0; i < CALLBACK_NUM; i++){
            if (sWakeUpCallback[i]) sWakeUpCallback[i]();
        }
        sIsSleeping = false;
    }
}

void Awake(){
    if(sIsSleepAcceptedCallbackCalled) 
        WakeUp();
}

void OrderToWaitForFinalize(){
    if(sIsSleeping){
        for(int i = 0; i < CALLBACK_NUM; i++){
            if (sFinalizeCallback[i]) sFinalizeCallback[i]();
        }
        sIsSleeping = false;
    }
}

bool RegisterSleepWakeUpCallback(void (*sleepCallback)(),void (*wakeUpCallback)(),void (*finalizeCallback)()){
    for (int i = 0; i < CALLBACK_NUM; i++){
        if (sSleepCallback[i] == NULL){
            NN_TASSERT_(sWakeUpCallback[i] == NULL);
            NN_TASSERT_(sFinalizeCallback[i] == NULL);
            sSleepCallback[i] = sleepCallback;
            sWakeUpCallback[i] = wakeUpCallback;
            sFinalizeCallback[i] = finalizeCallback;
            return true;
        }
    }
}

bool ClearSleepWakeUpCallback(void (*sleepCallback)(),void (*wakeUpCallback)(),void (*finalizeCallback)()){
    for (int i = 0; i < CALLBACK_NUM; i++){
        if (sSleepCallback[i] == sleepCallback){
            NN_TASSERT_(sWakeUpCallback[i] == wakeUpCallback);
            NN_TASSERT_(sFinalizeCallback[i] == finalizeCallback);
            sSleepCallback[i] = NULL;
            sWakeUpCallback[i] = NULL;
            sFinalizeCallback[i] = NULL;
            return true;
        }
    }
    return false;
}

}
}
}
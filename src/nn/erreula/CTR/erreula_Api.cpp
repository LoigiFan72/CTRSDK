// Filename: erreula_Api.cpp
//
// Project: Horizon

#include <nn/erreula/CTR/erreula_Api.h>

namespace nn{
namespace erreula{
namespace CTR{
namespace{
    bool CheckAppletRetry(Result result){
        if(!result.IsSuccess() || (result == ResultAlreadyExist())){
            NN_UTIL_PANIC_IF_FAILED(result);
            return true;
        }
        return false;
    }
}

void InitializeConfig(Config* pConfig){
    NN_POINTER_ASSERT(pConfig);
    pConfig->errorType = ERROR_TYPE_ERROR_CODE;
    pConfig->errorCode = ERROR_TYPE_ERROR_CODE;
    pConfig->upperScreenFlag = UPPER_SCREEN_NORMAL;
    pConfig->useLanguage = USE_LANGUAGE_DEFAULT;
    memset(pConfig->errorText,0,sizeof(pConfig->errorText)); // 0xed8 size, DAMN! This shit bigger then your mother is.
    pConfig->homeButton = true;
    pConfig->softwareReset = false;
    pConfig->appJump = false;
    memset(pConfig->pad0,0,sizeof(pConfig->pad0));
    pConfig->returnCode = RETURN_CODE_UNKNOWN;
    pConfig->eulaVersion = 0;
    memset(pConfig->pad1,0,sizeof(pConfig->pad1));
}

void StartErrEulaApplet(applet::CTR::AppletWakeupState* pWakeupState, Parameter* pParameter){
    s32 readLen = 0;
    applet::CTR::AppletId id = 0;

    Result res = applet::CTR::detail::PrepareToStartLibraryApplet(0x406);
    CheckAppletRetry(res);
    applet::CTR::detail::StartLibraryApplet(0x406,reinterpret_cast<u8*>(pWakeupState),0xf80,applet::CTR::HANDLE_NONE);
    *pWakeupState = applet::CTR::detail::WaitForStarting(&id, reinterpret_cast<u8*>(pParameter), 0xf80, &readLen, 0, applet::CTR::WAIT_INFINITE);
}
}
}
}
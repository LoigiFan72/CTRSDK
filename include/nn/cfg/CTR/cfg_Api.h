#pragma once

#include <nn/types.h>
#include <nn/cfg/CTR/cfg_LanguageCode.h>
#include <nn/cfg/CTR/cfg_UserInfo.h>

namespace nn {
namespace cfg {
namespace CTR {

const char PORT_NAME_USER[] = "cfg:u";
const char PORT_NAME_INIT[] = "cfg:i";
const char PORT_NAME_SYSTEM[] = "cfg:s";
    
void Initialize();
void Finalize();
void GetBirthday(Birthday* pBirthday);
CfgLanguageCode GetLanguage();

struct LanguageCfgData{
    u8 code;
};

extern LanguageCfgData LANGUAGE_CFG_DEFAULT;

struct DebugParamCfgData{
    union debugParam{
        u8 flags1;
        u8 param1;
    };

    bool dlpDebug;
    debugParam param;
    u8 fsLatencyParam;
    u8 rsv;
};

}
}
}
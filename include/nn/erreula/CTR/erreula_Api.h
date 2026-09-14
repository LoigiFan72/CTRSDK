#pragma once

#include <nn/applet.h>
#include <nn/types.h>

namespace nn{
namespace erreula{
namespace CTR{

enum
{
    ERROR_LANGUAGE_FLAG = 0x100,
    ERROR_WORD_WRAP_FLAG = 0x200
};

enum ErrorType
{
    ERROR_TYPE_ERROR_CODE = 0,
    ERROR_TYPE_ERROR_TEXT,
    ERROR_TYPE_EULA,
    ERROR_TYPE_EULA_FIRST_BOOT,
    ERROR_TYPE_EULA_DRAW_ONLY,
    ERROR_TYPE_AGREE,

    ERROR_TYPE_ERROR_CODE_LANGUAGE = ERROR_TYPE_ERROR_CODE | ERROR_LANGUAGE_FLAG,
    ERROR_TYPE_ERROR_TEXT_LANGUAGE = ERROR_TYPE_ERROR_TEXT | ERROR_LANGUAGE_FLAG,
    ERROR_TYPE_EULA_LANGUAGE = ERROR_TYPE_EULA | ERROR_LANGUAGE_FLAG,

    ERROR_TYPE_ERROR_TEXT_WORD_WRAP = ERROR_TYPE_ERROR_TEXT | ERROR_WORD_WRAP_FLAG,
    ERROR_TYPE_ERROR_TEXT_LANGAUAGE_WORD_WRAP = ERROR_TYPE_ERROR_TEXT | ERROR_LANGUAGE_FLAG | ERROR_WORD_WRAP_FLAG,

    ERROR_TYPE_MAX_BIT = (1u << 31)
};

enum
{
    UPPER_SCREEN_NORMAL = 0,
    UPPER_SCREEN_STEREO,

    UPPER_SCREEN_MAX
};

enum
{
    USE_LANGUAGE_DEFAULT = 0,
    USE_LANGUAGE_JAPANESE,
    USE_LANGUAGE_ENGLISH,
    USE_LANGUAGE_FRENCH,
    USE_LANGUAGE_GERMAN,
    USE_LANGUAGE_ITALIAN,
    USE_LANGUAGE_SPANISH,
    USE_LANGUAGE_SIMP_CHINESE,
    USE_LANGUAGE_KOREAN,
    USE_LANGUAGE_DUTCH,
    USE_LANGUAGE_PORTUGUESE,
    USE_LANGUAGE_RUSSIAN,
    USE_LANGUAGE_TRAD_CHINESE,
    
    USE_LANGUAGE_MAX
};

enum ReturnCode
{
    RETURN_CODE_UNKNOWN = -1,
    RETURN_CODE_NONE = 0,
    RETURN_CODE_SUCCESS,
    RETURN_CODE_NOT_SUPPORTED,

    RETURN_CODE_HOME_BUTTON = 10,
    RETURN_CODE_SOFTWARE_RESET,
    RETURN_CODE_POWER_BUTTON,

    RETURN_CODE_MAX_BIT = (s32)(1u << 31)
};

const u16 ERROR_TEXT_LENGTH_MAX = 1900;

struct Config
{
    ErrorType   errorType;
    s32         errorCode;
    s16         upperScreenFlag;
    u16         useLanguage;
    wchar_t     errorText[ERROR_TEXT_LENGTH_MAX];
    bool        homeButton;
    bool        softwareReset;
    bool        appJump;
    u8          pad0[137];
    ReturnCode  returnCode;
    u16         eulaVersion;
    u8          pad1[10];
};

struct Parameter
{
    Config  config;
};

void InitializeConfig(Config* pConfig);
void PreloadErrEulaApplet();
void CancelPreloadErrEulaApplet();
void WaitForPreloadErrEulaApplet();
void StartErrEulaApplet(applet::CTR::AppletWakeupState* pWakeupState, Parameter* pParameter);

}
}
}
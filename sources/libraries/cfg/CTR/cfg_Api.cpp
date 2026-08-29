// Filename: cfg_Api.cpp
//
// Project: Horizon

#include <nn/cfg/CTR/cfg_Api.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/detail/cfg_Default.h>
#include <nn/cfg/CTR/cfg_IpcUser.h>
#include <nn/cfg/CTR/detail/cfg_Default.h>
#include <nn/os.h>
#include <nn/Result.h>
#include <nn/err/CTR/err_Api.h>

namespace nn {
namespace cfg {
namespace CTR {

void Initialize(){
    Result res = detail::Initialize();
    NN_ERR_THROW_FATAL_ALL(res);
}

void Finalize(){
    detail::Finalize();
}

void GetBirthday(Birthday* pBirthday){
    nn::Result result;
    Birthday birthdayCfgData = detail::BIRTHDAY_CFG_DEFAULT;
    result = detail::GetConfig(&birthdayCfgData, sizeof(Birthday), 0xa0001);
    NN_ERR_THROW_FATAL_ALL(result);
    pBirthday->month = birthdayCfgData.month;
    pBirthday->day = birthdayCfgData.day;
}

CfgRegionCode GetRegion(){
    return detail::GetRegion();
}

CfgLanguageCode GetLanguage(){
    LanguageCfgData languageCode; Result res;
    languageCode.code = detail::LANGUAGE_CFG_DEFAULT.code;
    res = detail::IpcUser::GetConfig(&languageCode,sizeof(CfgLanguageCode),0xa0002);
    NN_ERR_THROW_FATAL_ALL(res);
    return (CfgLanguageCode)languageCode.code;
}

bit64 GetTransferableId(bit32 uniqueId){
    nn::Result result;
    bit64 transferableId;
    result = detail::GetTransferableId(uniqueId, &transferableId);
    NN_ERR_THROW_FATAL_ALL(result);
    return transferableId;
}

namespace{
    nn::os::CriticalSection s_CriticalSection = nn::WithInitialize();
}

}
}
}
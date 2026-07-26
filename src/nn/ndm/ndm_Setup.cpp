// Filename: ndm_Setup.cpp
//
// Project: Horizon

#include <nn/ndm.h>
#include <nn/applet/CTR/applet_Info.h>
#include <nn/applet/CTR/applet_API.h>

#include <nn/dbg/dbg_DebugString.h>

namespace nn{
namespace ndm{

void SetupDaemonsDefault(){
    if(applet::CTR::IsInitialized() && !applet::CTR::GetAppletType()){
        Result result = nn::ndm::Initialize();
        NN_UTIL_PANIC_IF_FAILED(result);

        result = nn::ndm::CTR::detail::Interface::OverrideDefaultDaemons(0xfu);
        if(result.IsFailure()){
            NN_TLOG_("Failed to override default daemons. (SDK version mismatch?)\n");
            NN_DBG_PRINT_RESULT(result);
        }

        result = nn::ndm::SuspendDaemons(6u);
        if(result.IsFailure()){
            NN_TLOG_("Failed to suspend boss daemon.\n");
            NN_DBG_PRINT_RESULT(result);
        }
    }
}

}
}
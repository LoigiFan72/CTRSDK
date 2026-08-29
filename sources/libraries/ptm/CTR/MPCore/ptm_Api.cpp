// Filename: ptm_Api.cpp
//
// Project: Horizon

#include <nn/ptm/CTR/ptm_Api.h>
#include <nn/ptm/CTR/detail/ptm_PtmIpc.h>
#include <nn/util/util_Result.h>
#include <nn/srv/srv_API.h>
#include <string.h>

namespace nn{
namespace ptm{
namespace CTR{


Result Initialize(){
    if(!detail::PtmIpc::s_Session.IsValid()){
        NN_UTIL_RETURN_IF_FAILED(srv::GetServiceHandle(&detail::PtmIpc::s_Session, PORT_NAME_PTM_USER));
    }
    return ResultSuccess();
}



Result Finalize(){
    if(detail::PtmIpc::s_Session.IsValid()){
        NN_UTIL_RETURN_IF_FAILED(svc::CloseHandle(detail::PtmIpc::s_Session));
        detail::PtmIpc::s_Session = INVALID_HANDLE_VALUE;
    }
    return ResultSuccess();
}
    
}
}
}
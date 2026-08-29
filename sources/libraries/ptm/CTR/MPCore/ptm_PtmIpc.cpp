// Filename: ptm_PtmIpc.cpp
//
// Project: Horizon

#include <nn/ptm/CTR/detail/ptm_PtmIpc.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace ptm{
namespace CTR{
namespace detail{

Handle PtmIpc::s_Session;

Result PtmIpc::GetStepHistory(u16 pStepCounts[], s32 numHours, fnd::DateTime start){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xB, 3, 2, 0);
    ipcMsg.SetRaw(1, numHours);
    ipcMsg.SetRaw(2, start);
    ipcMsg.SetReceive(4, pStepCounts, sizeof(*pStepCounts) * numHours);


    Result ipcResult = SendSyncRequest(s_Session);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}


}
}
}
}
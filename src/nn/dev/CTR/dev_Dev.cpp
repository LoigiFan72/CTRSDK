// Filename: dev_Dev.cpp
//
// Project: Horizon

#include <nn/dev/CTR/dev_Dev.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace dev{
namespace CTR{

Result Dev::ReadHostIO(bit8 pData[], size_t size, s32 numSectors, SectorSize sectorSize, const bit8 pCommnand[]){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(1, 7, 2, 0);
    ipcMsg.SetRaw(1, size);
    ipcMsg.SetRaw(2, numSectors);
    ipcMsg.SetRaw(3, sectorSize);
    ipcMsg.SetRawArray(4, pCommnand, sizeof(*pCommnand) * 15);
    ipcMsg.SetPXIOut(8, 0, pData, sizeof(*pData) * size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Dev::WriteHostIO(const bit8 pData[], size_t size, s32 numSectors, SectorSize sectorSize, const bit8 pCommnand[]){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(2, 7, 2, 0);
    ipcMsg.SetRaw(1, size);
    ipcMsg.SetRaw(2, numSectors);
    ipcMsg.SetRaw(3, sectorSize);
    ipcMsg.SetRawArray(4, pCommnand, sizeof(*pCommnand) * 15);
    ipcMsg.SetPXIIn(8, 0, pData, sizeof(*pData) * size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Dev::ReadHostIO2(bit8 pData[], size_t size, s32 sectorOffset, s32 numSectors, SectorSize sectorSize){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(3, 4, 2, 0);
    ipcMsg.SetRaw(1, size);
    ipcMsg.SetRaw(2, sectorOffset);
    ipcMsg.SetRaw(3, numSectors);
    ipcMsg.SetRaw(4, sectorSize);
    ipcMsg.SetPXIOut(5, 0, pData, sizeof(*pData) * size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Dev::WriteHostIO2( const bit8 pData[], size_t size, s32 sectorOffset, s32 numSectors, SectorSize sectorSize ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(4, 4, 2, 0);
    ipcMsg.SetRaw(1, size);
    ipcMsg.SetRaw(2, sectorOffset);
    ipcMsg.SetRaw(3, numSectors);
    ipcMsg.SetRaw(4, sectorSize);
    ipcMsg.SetPXIIn(5, 0, pData, sizeof(*pData) * size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}
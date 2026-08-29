// Filename: dev_Api.cpp
//
// Project: Horizon


#include <nn/dev/CTR/dev_Api.h>
#include <nn/dev/CTR/dev_Dev.h>
#include <nn/util/util_Result.h>
#include <nn/srv/srv_API.h>
#include <string.h>

namespace nn{
namespace dev{
namespace CTR{
namespace{
    Handle s_Dev9Handle;
    bool   s_Initialized;

    size_t CalcSize(s32 numSectors, SectorSize sectorSize){
        const size_t SIZE_TABLE[] = {0, 4, 16, 64, 512, 1024, 2048, 4096, 8192};
        return SIZE_TABLE[sectorSize] * numSectors;
    }
}
void Initialize(){
    Result res;
    if(!s_Initialized){
        srv::Initialize();
        NN_UTIL_PANIC_IF_FAILED(nn::srv::GetServiceHandle(&s_Dev9Handle, nn::pxi::CTR::PORT_NAME_DEV9));
        s_Initialized = true;
    }
}

void Finalize(){
    if(s_Initialized){
        NN_UTIL_PANIC_IF_FAILED(nn::svc::CloseHandle(s_Dev9Handle));
        s_Initialized = false;
    }
}

Result ReadHostIO(void* pData, s32 numSectors, SectorSize sectorSize, const bit8 pCommand[]){
    if(!s_Initialized){
        NN_TPANIC_("Not initialized");
    }

    u32 size = CalcSize(numSectors,sectorSize);
    Dev dev(s_Dev9Handle);
    return dev.ReadHostIO((u8*)pData, size, numSectors, sectorSize, (u8*)pCommand);
}

Result WriteHostIO(const void* pData, s32 numSectors,SectorSize sectorSize, const bit8 pCommand[]){
    if(!s_Initialized){
        NN_TPANIC_("Not initialized");
    }

    size_t size = CalcSize(numSectors, sectorSize);
    Dev dev(s_Dev9Handle);
    return dev.WriteHostIO(reinterpret_cast<const bit8*>(pData), size, numSectors, sectorSize, pCommand);
}

Result ReadHostIO2(void* pData, s32 sectorOffset, s32 numSectors, SectorSize sectorSize){
    if(!s_Initialized){
        NN_TPANIC_("Not initialized");
    }

    size_t size = CalcSize(numSectors, sectorSize);
    Dev dev(s_Dev9Handle);
    return dev.ReadHostIO2(reinterpret_cast<bit8*>(pData), size, sectorOffset, numSectors, sectorSize );
}

Result WriteHostIO2( const void* pData, s32 sectorOffset, s32 numSectors, SectorSize sectorSize){
    if(!s_Initialized){
        NN_TPANIC_("Not initialized");
    }

    size_t size = CalcSize(numSectors, sectorSize);
    Dev dev(s_Dev9Handle);
    return dev.WriteHostIO2(reinterpret_cast<const bit8*>(pData), size, sectorOffset, numSectors, sectorSize);
}

}
}
}
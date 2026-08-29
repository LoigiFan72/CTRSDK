#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/cfg/CTR/cfg_Api.h>
#include <nn/cfg/CTR/cfg_ApiInit.h>
#include <nn/cfg/CTR/cfg_ApiSys.h>

namespace nn {
namespace cfg {
namespace CTR {
namespace detail {

typedef enum _IPCPortType{
        PORT_CFG_USER,
        PORT_CFG_SYS,
        PORT_CFG_INIT,
        PORT_CFG_NOR,
        NUM_OF_IPC_PORT
} IPCPortType;

Result Initialize();
Result InitializeBase(nn::Handle* session, const char* portName);
Result InitializeProperPort(IPCPortType* pPortType);
        
void Finalize();
Result FinalizeBase(nn::Handle* session);
void FinalizeProperPort(IPCPortType portType);

CfgRegionCode GetRegion();
Result GetConfig(void* pData, size_t size, bit32 key);

Result GetTransferableId(bit32 uniqueId, bit64* transferableId);

}
}
}
}
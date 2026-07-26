#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/cfg/CTR/cfg_RegionCode.h>

namespace nn {
namespace cfg {
namespace CTR {
namespace detail {

class IpcUser{
public:

static nn::Handle sSession;

static Result GetConfig (void* pData, size_t size, bit32 key);
static Result GetRegion (CfgRegionCode* regionCode);

};

}
}
}
}
// Filename: pl_PedometerApi.cpp
//
// Project: Horizon

#include <nn/pl/CTR/pl_PedometerApi.h>
#include <nn/ptm/CTR/detail/ptm_PtmIpc.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/util/util_Result.h>
#include <nn/Assert.h>

namespace nn {
namespace pl {
namespace CTR {

void GetStepHistory(ushort* pStepCounts, s32 numHours, nn::fnd::DateTime start){
    NN_UTIL_PANIC_IF_FAILED(nn::ptm::CTR::detail::PtmIpc::GetStepHistory(pStepCounts, numHours, start));
}

}
}
}
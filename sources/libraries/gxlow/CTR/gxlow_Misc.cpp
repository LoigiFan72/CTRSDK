// Filename: gxlow_Misc.cpp
//
// Project: Horizon


#pragma once

#include <nn/assert.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>
#include <nn/gxlow/CTR/gxlow_Misc.h>
#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/gxlow/CTR/gxlow_Result.h>

namespace nn {
namespace gxlow {
namespace CTR {

uptr GetPhysicalAddr(uptr virtualAddr){
    uptr physAddr;
    if(detail::IsContinuousMemory(virtualAddr)){
        physAddr = virtualAddr + 0xC000000;
    }
    else{
        if(detail::IsVram(virtualAddr) || (virtualAddr == 0x1f600000))
            physAddr = virtualAddr + 0xF9000000;
        else{
            NN_TASSERTMSG_(0,"[gx] Address %08X is neither on the device memory nor the VRAM.\n",virtualAddr);
        }
    }
    return physAddr;
}

void FlushDataCache(const void* pData, size_t size){
    uptr dataAddr = reinterpret_cast<uptr>(pData);
    NN_GX_ASSERT_DEVICE_MEMORY(dataAddr);
    NN_GX_ASSERT_DEVICE_MEMORY(dataAddr+size-1);
    nn::Result result;
    result = detail::GetGpuIpc()->FlushDataCache(nn::PSEUDO_HANDLE_CURRENT_PROCESS,dataAddr,size);
    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::FlushDataCache]");
}



}
}
}
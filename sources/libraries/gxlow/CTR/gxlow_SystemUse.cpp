// Filename: gxlow_SystemUse.cpp
//
// Project: Horizon

#include <nn/gxlow/CTR/gxlow_CTR.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>
#include <nn/gxlow/CTR/gxlow_SystemUse.h>
#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/gxlow/CTR/gxlow_InterruptReceiver.h>
#include <nn/gxlow/CTR/gxlow_Result.h>
#include <nn/assert.h>

namespace nn {
namespace gxlow {
namespace CTR {

Result AcquireGpuRight(){
    if (IsInitialized()){
        nn::Result result;
        result = detail::GetGpuIpc()->AcquireRight(nn::PSEUDO_HANDLE_CURRENT_PROCESS, false);
        if (result.IsSuccess() && detail::IsAppletMode() ){
            SuppressVSync(false);
        }
        return result;
    }
    else{
        return ResultLibNotInitialized();
    }
}

Result ReleaseGpuRight(){
    if (IsInitialized()){
        Result result;
        result = detail::GetGpuIpc()->ReleaseRight();
        if ( result.IsSuccess() && detail::IsAppletMode() ){
            SuppressVSync(true);
        }
        return result;
    }
    else{
        return ResultLibNotInitialized();
    }
}

Result SaveVramSysArea(){
    if (IsInitialized()){
        return detail::GetGpuIpc()->SaveVramSysArea();
    }
    else{
        return ResultLibNotInitialized();
    }
}

Result RestoreVramSysArea(){
    if (IsInitialized()){
        return detail::GetGpuIpc()->RestoreVramSysArea();
    }
    else{
        return ResultLibNotInitialized();
    }
}

void ImportDisplayCaptureInfo(DisplayCaptureInfo* info){
    NN_TASSERT_(info != 0);

    Result result;
    
    result = detail::GetGpuIpc()->ImportDisplayCaptureInfo(info);
    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::ImportDisplayCaptureInfo]");
    
    if ((info->surface[0].mode & (0x70)) == 0){
        info->surface[0].mode = FORMAT_UNIMPORTABLE;
    }
    else{
        if((info->surface[0].mode & (0x20)) == 0){
            info->surface[0].addrB = info->surface[0].addr;
        }
        info->surface[0].mode = static_cast<DisplayBufferMode>(info->surface[0].mode & (FORMAT_R4G4B4A4 | FORMAT_R5G5B5A1));
    }
    
    info->surface[1].mode = static_cast<DisplayBufferMode>(info->surface[1].mode & (FORMAT_R4G4B4A4 | FORMAT_R5G5B5A1));
    
    if(info->surface[0].addr == 0){
        info->surface[0].mode = FORMAT_UNIMPORTABLE;
    }
    
    if(info->surface[0].addrB == 0){
        info->surface[0].mode = FORMAT_UNIMPORTABLE;
    }
    
    if(info->surface[1].addr == 0){
        info->surface[1].mode = FORMAT_UNIMPORTABLE;
    }
    info->surface[1].addrB = info->surface[1].addr;
}

void SuppressVSync(bool enable){
    detail::GetInterruptReceiver()->SuppressPdcEvents(enable);
}



}
}
}
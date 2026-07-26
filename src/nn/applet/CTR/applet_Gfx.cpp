// Filename: applet_Gfx.cpp
//
// Project: Horizon

#include <nn/applet/CTR/applet_Connect.h>
#include <nn/applet/CTR/applet_API.h>
#include <nn/applet/CTR/applet_Ipc.h>
#include <nn/srv/srv_Api.h>
#include <nn/err/CTR/err_Api.h>
#include <nn/gxlow/CTR/gxlow_SystemUse.h>
#include <nn/gx/CTR/gx_Lcd.h>

#include <string.h>

namespace nn{
namespace applet{
namespace CTR{
namespace detail{

size_t GetByteSizePerPixel(const DisplayBufferMode mode){
    int mul = 0;
    switch (mode) {
        case FORMAT_R8G8B8A8:
            mul=4;
            break;
        case FORMAT_R8G8B8:
            mul=3;
            break;
        case FORMAT_R5G6B5:
        case FORMAT_R5G5B5A1:
        case FORMAT_R4G4B4A4:            
            mul=2;
            break;
        default: break;
    }

    return mul;
}

void GetDisplayInfo(AppletDisplayInfo* pInfo){
    if(!pInfo)
        return;
    gxlow::CTR::DisplayCaptureInfo infoTmp;

    nn::gxlow::CTR::ImportDisplayCaptureInfo(&infoTmp);
    pInfo->d[0].addr = infoTmp.surface[0].addr;
    pInfo->d[0].addrB = infoTmp.surface[0].addrB;
    pInfo->d[1].addr = infoTmp.surface[1].addr;
    pInfo->d[1].addrB = infoTmp.surface[1].addrB;
    pInfo->d[0].mode = static_cast<AppletDisplayBufferMode>(infoTmp.surface[0].mode);
    pInfo->d[1].mode = static_cast<AppletDisplayBufferMode>(infoTmp.surface[1].mode);
    pInfo->d[0].stride = infoTmp.surface[0].stride;
    pInfo->d[1].stride = infoTmp.surface[1].stride;
}

void CalcCaptureBufferInfo(CaptureBufferInfo *cInfo){
    int x = 256;
    size_t bufferSize = 0;
    size_t pixelSize;

    {
        pixelSize = GetByteSizePerPixel(cInfo->d[1].mode);
        cInfo->d[1].offset = cInfo->d[1].offsetB = 0;
        bufferSize += x * NN_GX_DISPLAY0_HEIGHT * ((pixelSize > 3) ? 3 : pixelSize);
    }

    {
        pixelSize = GetByteSizePerPixel(cInfo->d[0].mode);
        cInfo->d[0].offset = cInfo->d[0].offsetB = bufferSize;
        bufferSize += x * NN_GX_DISPLAY0_HEIGHT * ((pixelSize > 3) ? 3 : pixelSize);
    }

    if (cInfo->is3DCapture){
        pixelSize = GetByteSizePerPixel(cInfo->d[0].mode);
        cInfo->d[0].offsetB = bufferSize;
        bufferSize += x * NN_GX_DISPLAY0_HEIGHT * ((pixelSize > 3) ? 3 : pixelSize);
    }

    bufferSize += x * (512 - NN_GX_DISPLAY0_HEIGHT) * ((pixelSize > 3) ? 3 : pixelSize);
    cInfo->size = bufferSize;
}

}
}
}
}
#pragma once

#include <nn/Result.h>
#include <nn/gxlow/CTR/gxlow_Parameters.h>

#ifdef __cplusplus

namespace nn{
namespace gxlow{
namespace CTR{
    Result AcquireGpuRight();
    void ImportDisplayCaptureInfo(DisplayCaptureInfo* pInfo);
    Result ReleaseGpuRight();
    Result RestoreVramSysArea();
    Result SaveVramSysArea();
    void SuppressVSync(bool enable);
}
}
}

#endif
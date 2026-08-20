// Filename: rocrt_Control.cpp
//
// Project: Horizon
//
// Custom, filename not confirmed. 

#include <nn/ro.h>
#include <nn/module.h>
#include <nn/ro/ro_Info.h>

extern "C"{
    extern __weak bit8 Image$$CODE_INIT_ARRAY$$Base[];
    extern __weak bit8 Image$$CODE_INIT_ARRAY$$Limit[];
    extern __weak bit8 Image$$RO_STATIC_INIT$$Base[];
    extern __weak bit8 Image$$RO_STATIC_INIT$$Limit[];
}

namespace{

#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)
    NN_MAKE_MODULE(sDebugIndicator,  "NINTENDO", "DEBUG");
#endif
    NN_MAKE_MODULE(sSdkVersion,      "NINTENDO", NN_CURRENT_SDK_VERSION);
    NN_MAKE_MODULE(sFirmwareVersion, "NINTENDO", NN_CURRENT_FIRMWARE_VERSION);

void* spWorkArea;

const CodeRegion sInitArray ={
    reinterpret_cast<uptr>(Image$$CODE_INIT_ARRAY$$Base),
    reinterpret_cast<uptr>(Image$$CODE_INIT_ARRAY$$Limit)
};

const CodeRegion sStaticInitArray ={
    reinterpret_cast<uptr>(Image$$RO_STATIC_INIT$$Base),
    reinterpret_cast<uptr>(Image$$RO_STATIC_INIT$$Limit)
};

asm void ReferSymbol(const void* sym, ...){
    bx  lr
}

}

extern "C"{


extern __weak detail::EitLinkNode nnroEitNode_;

Result nnroControlObject_(void* param, ObjectControl c){
    switch(c){
    case OBJECT_CONTROL_GET_EIT_NODE:{
        detail::EitLinkNode** ppNode = reinterpret_cast<detail::EitLinkNode**>(param);
        *ppNode = &nnroEitNode_;
    }
    break;

    case OBJECT_CONTROL_SET_WORK_AREA:{
        spWorkArea = param;
    }
    break;

    case OBJECT_CONTROL_GET_WORK_AREA:{
        void** ppWork = reinterpret_cast<void**>(param);
        *ppWork = spWorkArea;
    }
    break;

    case OBJECT_CONTROL_GET_INIT_ARRAY:{
        reinterpret_cast<CodeRegion*>(param)->begin = sInitArray.begin;
        reinterpret_cast<CodeRegion*>(param)->end = sInitArray.end;
    }
    break;

    case OBJECT_CONTROL_GET_STATIC_INIT_ARRAY:{
        reinterpret_cast<CodeRegion*>(param)->begin = sStaticInitArray.begin;
        reinterpret_cast<CodeRegion*>(param)->end = sStaticInitArray.end;
    }
    default:
#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)
        ReferSymbol(sDebugIndicator);
#endif
        ReferSymbol(sSdkVersion);
        ReferSymbol(sFirmwareVersion);
        return ResultUnknownObjectControl();
    }

    return nn::ResultSuccess();

}

}
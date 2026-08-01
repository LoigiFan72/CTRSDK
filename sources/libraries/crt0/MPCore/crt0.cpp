// Filename: [MPCORE] crt0.cpp
//
// Project: Horizon

#include <nn/svc.h>
#include <nn/init/init_Default.h>
#include <nn/init/init_StartUp.h>
#include <nn/init.h>
#include <nn/module.h>
#include <nn/version.h>
#include <nn/util/detail/util_Symbol.h>
#include <rt_locale.h>
#include <rt_sys.h>

namespace{
#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)
    NN_MAKE_MODULE(sDebugIndicator,  "NINTENDO", "DEBUG");
#endif
    NN_MAKE_MODULE(sSdkVersion,      "NINTENDO", NN_CURRENT_SDK_VERSION);
    NN_MAKE_MODULE(sFirmwareVersion, "NINTENDO", NN_CURRENT_FIRMWARE_VERSION);
}

extern "C"{
    void nninitRegion();
    void nninitLocale();

    __weak void __cpp_initialize__aeabi_(void);
    bit32* __rt_locale(void);
    void _fp_init(void);

    extern u8 Image$$ZI$$ZI$$Limit[];
    extern u8 Image$$ZI$$ZI$$Base[];

#pragma arm
asm void __ctr_start(){
    PRESERVE8
    bl __cpp(nninitRegion) // Region
    bl __cpp(nninitLocale) // Locale
    bl __cpp(nninitSystem) // System
    bl __cpp(nninitStartUp) // Startup
    blx __cpp(__cpp_initialize__aeabi_) // Initialize CPP ARM
    bl __cpp(nninitCallStaticInitializers) // Static Initializer Manager
    bl __cpp(nninitSetup) // Initializes Setup
    bl __cpp(nnMain) // Main Application Loop
    b __cpp(nn::svc::ExitProcess) // Exit Process if needed
}

void nninitLocale(){
#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)
    NN_REFER_MODULE(sDebugIndicator);
#endif
    NN_REFER_MODULE(sSdkVersion);
    NN_REFER_MODULE(sFirmwareVersion);

    bit32* p = __rt_locale();
    *(p + 1) = (bit32)_get_lc_ctype(0, 0) + 1;
    *(p + 3) = (bit32)_get_lc_numeric(0, 0);
}

#pragma arm

asm void nninitRegion(){
    ldr     r0,=__cpp(Image$$ZI$$ZI$$Base)
    ldr     r1,=__cpp(Image$$ZI$$ZI$$Limit)
    mov     r2,#0x0
loop
    cmp     r0,r1
    strcc   r2,[r0],#4
    bcc     loop
    bx      lr
};

} // extern "C"
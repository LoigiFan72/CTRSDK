#pragma once

#include <nn/os.h>
#include <nn/dbg.h>
#include <nn/gxlow/CTR/gxlow_Result.h>
#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/os/CTR/MPCore/os_MemoryMap.h>
#include <nn/os/os_SharedInfo.h>

#define NN_GX_VRAM_AREA_SIZE          (0x00300000)
#define NN_GX_VRAMA_START             (0x08000000)
#define NN_GX_VRAMA_END               (NN_GX_VRAMA_START+NN_GX_VRAM_AREA_SIZE-1)
#define NN_GX_VRAMB_START             (NN_GX_VRAMA_START+NN_GX_VRAM_AREA_SIZE)
#define NN_GX_VRAMB_END               (NN_GX_VRAMB_START+NN_GX_VRAM_AREA_SIZE-1)

#define NN_GX_VRAMA_SYS_AREA_SIZE     (0x00180000)
#define NN_GX_VRAMB_SYS_AREA_SIZE     (0x00280000)
#define NN_GX_VRAM_USER_AREA_SIZE     (0x00300000)
#define NN_GX_VRAM_DB_SAFE_AREA_SIZE  (0x00180000)

#define NN_GX_VRAMA_SYS_START         (NN_GX_VRAMA_END-NN_GX_VRAMA_SYS_AREA_SIZE+1)
#define NN_GX_VRAMA_SYS_END           (NN_GX_VRAMA_END)
#define NN_GX_VRAMA_USER_START        (NN_GX_VRAMA_START)
#define NN_GX_VRAMA_USER_END          (NN_GX_VRAMA_END)
#define NN_GX_VRAMA_DB_SAFE_AREA_END  (NN_GX_VRAMA_START+NN_GX_VRAM_DB_SAFE_AREA_SIZE)

#define NN_GX_VRAMB_SYS_START         (NN_GX_VRAMB_END-NN_GX_VRAMB_SYS_AREA_SIZE+1)
#define NN_GX_VRAMB_SYS_END           (NN_GX_VRAMB_END)
#define NN_GX_VRAMB_USER_START        (NN_GX_VRAMB_START)
#define NN_GX_VRAMB_USER_END          (NN_GX_VRAMB_END)
#define NN_GX_VRAMB_DB_SAFE_AREA_END  (NN_GX_VRAMB_START+NN_GX_VRAM_DB_SAFE_AREA_SIZE)

#define NN_GXLOW_RESULT_ASSERT(result, msg)         \
    do                                              \
    {                                               \
        if ( ! result.IsSuccess() )                 \
        {                                           \
            NN_DBG_PRINT_RESULT(result);            \
            NN_TASSERTMSG_(                         \
                result == nn::gxlow::CTR::ResultNoRightAcquired(),  \
                msg                                 \
            );                                      \
        }                                           \
    } while(0)


#define NN_GX_ASSERT_DEVICE_MEMORY(addr)                        \
    NN_TASSERTMSG_(                                             \
        ::nn::gxlow::CTR::detail::IsContinuousMemory(addr),     \
        "[gx] Address %08X is not on the device memory.\n",     \
        addr                                                    \
    )

#define NN_GX_ASSERT_VRAM(addr)                         \
    NN_TASSERTMSG_(                                     \
        ::nn::gxlow::CTR::detail::IsVram(addr),         \
        "[gx] Address %08X is not on the VRAM.\n",      \
        addr                                            \
    )

#define NN_GX_ASSERT_DEVICE_MEMORY_OR_VRAM(addr)                                \
    NN_TASSERTMSG_(                                                             \
        ::nn::gxlow::CTR::detail::IsContinuousMemory(addr) ||                   \
        ::nn::gxlow::CTR::detail::IsVram(addr),                                 \
        "[gx] Address %08X is neither on the device memory nor the VRAM.\n",    \
        addr                                                                    \
    )
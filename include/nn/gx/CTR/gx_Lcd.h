#pragma once

#include "nn/types.h"
#include "nn/assert.h"

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef NN_GX_DISPLAY0_WIDTH
#define NN_GX_DISPLAY0_WIDTH    240
#endif

#ifndef NN_GX_DISPLAY0_HEIGHT
#define NN_GX_DISPLAY0_HEIGHT   400
#endif

#ifndef NN_GX_DISPLAY1_WIDTH
#define NN_GX_DISPLAY1_WIDTH    240
#endif

#ifndef NN_GX_DISPLAY1_HEIGHT
#define NN_GX_DISPLAY1_HEIGHT   320
#endif

void nngxStartLcdDisplay( void );

#ifdef  __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef  __cplusplus
namespace nn {
namespace gx {
namespace CTR {

enum DisplaySize
{
    DISPLAY0_WIDTH  = NN_GX_DISPLAY0_WIDTH,
    DISPLAY0_HEIGHT = NN_GX_DISPLAY0_HEIGHT,
    DISPLAY1_WIDTH  = NN_GX_DISPLAY1_WIDTH,
    DISPLAY1_HEIGHT = NN_GX_DISPLAY1_HEIGHT
};

inline void StartLcdDisplay()
{
    nngxStartLcdDisplay();
}


}
}
}
#endif // __cplusplus
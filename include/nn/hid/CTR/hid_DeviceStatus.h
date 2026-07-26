#pragma once

#include <nn/types.h>
#include <nn/math/math_Vec3.h>
#include <nn/math/math_Matrix33.h>

namespace nn {
namespace hid {
namespace CTR {
    const bit32 BUTTON_A                = 0x00000001;
    const bit32 BUTTON_B                = 0x00000002;
    const bit32 PSEUDO_BUTTON_SELECT    = 0x00000004;
    const bit32 BUTTON_SELECT           = 0x00000004;
    const bit32 BUTTON_START            = 0x00000008;
    const bit32 BUTTON_RIGHT            = 0x00000010;
    const bit32 BUTTON_LEFT             = 0x00000020;
    const bit32 BUTTON_UP               = 0x00000040;
    const bit32 BUTTON_DOWN             = 0x00000080;
    const bit32 BUTTON_R                = 0x00000100;
    const bit32 BUTTON_L                = 0x00000200;
    const bit32 BUTTON_X                = 0x00000400;
    const bit32 BUTTON_Y                = 0x00000800;
    const bit32 BUTTON_DEBUG            = 0x00001000;
    const bit32 BUTTON_RESERVED         = 0x00002000;
    const bit32 BUTTON_EMULATION_RIGHT  = 0x10000000;
    const bit32 BUTTON_EMULATION_LEFT   = 0x20000000;
    const bit32 BUTTON_EMULATION_UP     = 0x40000000;
    const bit32 BUTTON_EMULATION_DOWN   = 0x80000000;

    typedef struct{
        s16 x;
        s16 y;
    } AnalogStickStatus;

    typedef struct{
        bit32 hold;
        bit32 trigger;
        bit32 release;
        AnalogStickStatus stick;
    } PadStatus;

    typedef struct{
        u16 x;
        u16 y;
        u8 touch;
        s8 rev[3];
    } TouchPanelStatus;

    typedef struct{
        s16 x;
        s16 y;
        s16 z;
    }AccelerometerStatus;


    typedef struct{
        f32 x;
        f32 y;
        f32 z;
    }AccelerationFloat;

    struct Direction{
    public:
        nn::math::VEC3 x;
        nn::math::VEC3 y;
        nn::math::VEC3 z;
    public: 
        Direction(){ }
        explicit Direction(const nn::math::VEC3& vecx,const nn::math::VEC3& vecy,const nn::math::VEC3& vecz):x(vecx),y(vecy),z(vecz){}
        Direction(const nn::math::MTX33 &mtx33)//:
            //x(mtx33.v[0]),y(mtx33.v[1]),z(mtx33.v[2])
        {}

        nn::math::MTX33 ToMTX33(){
            return nn::math::MTX33(
                x.x, x.y, x.z,
                y.x, y.y, y.z,
                z.x, z.y, z.z);
        }
    };

    typedef struct{
        nn::math::VEC3  speed;
        nn::math::VEC3  angle;
        Direction       direction;
    }GyroscopeStatus;

    const bit16 DEBUG_PAD_BUTTON_UP           = 0x00000001;
    const bit16 DEBUG_PAD_BUTTON_LEFT         = 0x00000002;
    const bit16 DEBUG_PAD_TRIGGER_ZR          = 0x00000004;
    const bit16 DEBUG_PAD_BUTTON_X            = 0x00000008;
    const bit16 DEBUG_PAD_BUTTON_A            = 0x00000010;
    const bit16 DEBUG_PAD_BUTTON_Y            = 0x00000020;
    const bit16 DEBUG_PAD_BUTTON_B            = 0x00000040;
    const bit16 DEBUG_PAD_TRIGGER_ZL          = 0x00000080;
    const bit16 DEBUG_PAD_RESERVED            = 0x00000100;
    const bit16 DEBUG_PAD_TRIGGER_R           = 0x00000200;
    const bit16 DEBUG_PAD_BUTTON_PLUS         = 0x00000400;
    const bit16 DEBUG_PAD_BUTTON_HOME         = 0x00000800;
    const bit16 DEBUG_PAD_BUTTON_MINUS        = 0x00001000;
    const bit16 DEBUG_PAD_TRIGGER_L           = 0x00002000;
    const bit16 DEBUG_PAD_BUTTON_DOWN         = 0x00004000;
    const bit16 DEBUG_PAD_BUTTON_RIGHT        = 0x00008000;

    typedef struct{
        bit16 hold;
        bit16 trigger;
        bit16 release;
        short rev;
        f32 leftStickX;
        f32 leftStickY;
        f32 rightStickX;
        f32 rightStickY;
    } DebugPadStatus;
}
}
}
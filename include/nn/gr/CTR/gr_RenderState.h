#pragma once

#include <nn/gr/CTR/gr_Utility.h>

namespace nn{
namespace gr{
namespace CTR{

class RenderState{
public:
    enum ColorMask{
        COLOR_MASK_R    = 1 << 0,
        COLOR_MASK_G    = 1 << 1,
        COLOR_MASK_B    = 1 << 2,
        COLOR_MASK_A    = 1 << 3, 
        COLOR_MASK_RGBA = COLOR_MASK_R | COLOR_MASK_G | COLOR_MASK_B | COLOR_MASK_A 
    };

    class AlphaTest{
    public:
        bool isEnable;
        u8 refValue;
        PicaDataAlphaTest func;
        s8 rev;

        explicit AlphaTest(const RenderState& renderState_);
        bit32* MakeCommand(bit32* command, bool isUpdateFBAccess = true) const;
    protected:
        const RenderState& mRenderState;
    };

    class Blend{
    public:
        bool isEnable;
        PicaDataBlendEquation eqRgb;
        PicaDataBlendEquation eqAlpha;
        PicaDataBlendFunc srcRgb;
        PicaDataBlendFunc srcAlpha;
        PicaDataBlendFunc dstRgb;
        PicaDataBlendFunc dstAlpha;
        u8 colorR;
        u8 colorG;
        u8 colorB;
        u8 colorA;
        s8 rev;

        explicit Blend(const RenderState& renderState_);
        bit32* MakeCommand(bit32* command, bool isUpdateFBAccess = true) const;
        static bit32* MakeDisableCommand(bit32* command, bool isClearFrameBufferCache = true);
    protected:
        const RenderState& mRenderState;
    };

    class LogicOp{
    public:
        bool isEnable;
        PicaDataLogicOp opCode;

        s8 rev[2];

        explicit LogicOp(const RenderState& renderState_);
        bit32* MakeCommand(bit32* command, bool isUpdateFBAccess = true) const;

    protected:
        const RenderState& mRenderState;
    };

    class ShadowMap{
    public:
        explicit ShadowMap(const RenderState& renderState_);
        bit32* MakeCommand(bit32* command,bool isUpdateFBAccess = true,bool isAddDummyCommand = true) const;
        bit32* MakeAttenuationCommand(bit32* command) const;
        bit32* MakeTextureCommand(bit32* command,bool isAddDummyCommand = true) const;


        bool isEnable;
        bool isPerspective;
        s8 rev[2];
        f32 zBias;
        f32 zScale;
        f32 penumbraScale;
        f32 penumbraBias;

    protected:
        const RenderState& mRenderState;
    };

    class StencilTest{
    public:
        bool isEnable;
        bit8 maskOp;
        s8 rev[2];
        PicaDataStencilTest func;
        s8 rev2[3];
        bit32  ref;
        bit32  mask;
        PicaDataStencilOp opFail;
        PicaDataStencilOp  opZFail;
        PicaDataStencilOp  opZPass;
        s8 rev3;

        explicit StencilTest( const RenderState& renderState_ );
        bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;

    protected:
        const RenderState& mRenderState;
    };

    class DepthTest{
    public:
        bool             isEnable;
        bool             isEnableWrite;
        PicaDataDepthTest func;
        s8 rev;

        explicit DepthTest(const RenderState& renderState_);
        bit32* MakeCommand(bit32* command, bool isUpdateFBAccess = true) const;

    protected:
        const RenderState& mRenderState;
    };

    class WBuffer{
    public:
        f32 wScale;
        bool isEnablePolygonOffset;
        s8 rev[3];
        f32  polygonOffsetUnit;
        f32  depthRangeNear;
        f32  depthRangeFar;
        u8   depthRangeBit;
        s8 rev342r[3];

        explicit WBuffer();
        bit32* MakeCommand(bit32* command) const;
    };

    class Culling{
    public:
        bool isEnable;

        enum FrontFace{
            FRONT_FACE_CW, 
            FRONT_FACE_CCW
        };

        FrontFace frontFace;

        enum CullFace{
            CULL_FACE_FRONT,
            CULL_FACE_BACK
        };

        CullFace cullFace;
        s8 rev;

        explicit Culling(const RenderState& renderState_);
        bit32* MakeCommand( bit32* command, bool isUpdateFBAccess = true ) const;

    protected:
        const RenderState& mRenderState;
    };


    class FBAccess{
    public:
        explicit FBAccess(const RenderState& renderState_);
        bit32* MakeCommand(bit32* command, bool isClearFrameBufferCache = true) const;
        static bit32* MakeDisableCommand(bit32* command, bool isClearFrameBufferCache = true);
        static bit32* MakeClearCacheCommand(bit32* command);

    protected:
        const RenderState& mRenderState;
    };

    Blend       blend;
    LogicOp     logicOp;
    ShadowMap  shadowMap;
    AlphaTest   alphaTest;
    StencilTest stencilTest;
    ColorMask   colorMask;
    s8 rev[3];
    DepthTest   depthTest;
    Culling     cullingTest;
    WBuffer     wBuffer;
    FBAccess    fbAccess;

    explicit RenderState();
    bit32* MakeCommand(bit32* command, bool isClearFrameBufferCache = true) const;
    static bit32* MakeDisableCommand( bit32* command, bool isClearFrameBufferCache = true );
};

}
}
}
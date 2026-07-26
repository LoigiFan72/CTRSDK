#pragma once

#include <nn/gr/CTR/gr_Utility.h>
#include <nn/gx/CTR/gx_PicaFragment.h>

namespace nn{
namespace gr{
namespace CTR{

class FragmentLight{
public:
    static const int MAX_SOURCES = 8;
    
    class Source{
    public:
        u8 id;
        bool isEnableTwoSideDiffuse;
        bool isEnableGeomFactor0;
        bool isEnableGeomFactor1;
        u8 diffuseR;
        u8 diffuseG;
        u8 diffuseB;
        u8 ambientR;
        u8 ambientG;
        u8 ambientB;
        u8 specular0R;
        u8 specular0G;
        u8 specular0B;
        u8 specular1R;
        u8 specular1G;
        u8 specular1B;
        bit32 posXY;
        bit32 posZ;
        u8 isInfinity;
        s8 rev[3];
        bit32 distAttnBias;
        bit32 distAttnScale;
        bit32 spotDirectionXY;
        bit32 spotDirectionZ;

        explicit Source();
    };

    u8 globalAmbientR;
    u8 globalAmbientG;
    u8 globalAmbientB;
    u8 rev;
    Source source[MAX_SOURCES];
    bool isEnable[MAX_SOURCES];
    bool isEnableSpot[MAX_SOURCES];
    bool isEnableDistAttn[MAX_SOURCES];
    bool isShadowed[MAX_SOURCES];
    PicaDataFragLightEnvLayerConfig layerConfig;
    PicaDataFragLightEnvFresnel fresnelSelector;
    PicaDataFragLightEnvTexture     shadowSelector;
    PicaDataFragLightEnvBump        bumpMode;
    PicaDataFragLightEnvTexture     bumpSelector;
    bool isEnableShadowPrimary;
    bool isEnableShadowSecondary;
    bool isEnableShadowAlpha;
    bool isInvertShadow;
    bool isEnableBumpRenorm;
    bool isEnableClampHighLights;
    bool isEnableLutD0;
    bool isEnableLutD1;
    bool isEnableLutRefl;
    short rev2;

    class LutConfig{
    public:
        PicaDataFragLightEnvLutInput input;
        bool isAbs;
        PicaDataFragLightEnvLutScale scale;
        s8 rev;

        explicit LutConfig();
    };

    LutConfig lutConfigD0;
    LutConfig lutConfigD1;
    LutConfig lutConfigSP;
    LutConfig lutConfigFR;
    LutConfig lutConfigRB;
    LutConfig lutConfigRG;
    LutConfig lutConfigRR;

    FragmentLight();
    static bit32* MakeDisableCommand(bit32* command, bool isAddDummyCommand = true);
};

}
}
}
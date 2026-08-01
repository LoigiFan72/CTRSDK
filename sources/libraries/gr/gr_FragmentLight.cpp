// Filename: gr_FragmentLight.cpp
//
// Project: Horizon

#include <nn/gr/CTR/gr_FragmentLight.h>

namespace nn{
namespace gr{
namespace CTR{

FragmentLight::Source::Source(): 
    id(0),
    isEnableTwoSideDiffuse(false),
    isEnableGeomFactor0(false),
    isEnableGeomFactor1(false),
    diffuseR(255),
    diffuseG(255),
    diffuseB(255),
    ambientR(0),
    ambientG(0),
    ambientB(0),
    specular0R(255),
    specular0G(255),
    specular0B(255),
    specular1R(255),
    specular1G(255),
    specular1B(255),
    posXY(0),
    posZ(0),
    isInfinity(0),
    distAttnBias(0),
    distAttnScale(0),
    spotDirectionXY(0),
    spotDirectionZ(0)
{}


bit32* FragmentLight::MakeDisableCommand( bit32* command, bool isAddDummyCommand ){
    const u32 lightNum = 0;

    if (isAddDummyCommand){
        *command++ = 0x0;
        *command++ = PICA_CMD_HEADER_BURST_BE(PICA_REG_TEXTURE_FUNC, 0x3, 0x0);

        *command++ = 0x0;
        *command++ = 0x0;
    }

    *command++ = 0;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_FRAG_LIGHT_EN0);
    
    *command++ = PICA_CMD_DATA_FRAG_LIGHT_EN_INV(lightNum);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_FRAG_LIGHT_EN1);

    *command++ = lightNum;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_FRAG_LIGHT_SRC_NUM);

    return command;
}

FragmentLight::FragmentLight(): 
    globalAmbientR(0),
    globalAmbientG(0),
    globalAmbientB(0),
    layerConfig(PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG0),
    fresnelSelector(PICA_DATA_FRAG_LIGHT_ENV_NO_FRESNEL),
    shadowSelector(PICA_DATA_FRAG_LIGHT_ENV_TEXTURE0),
    bumpMode(PICA_DATA_FRAG_LIGHT_ENV_BUMP_NOT_USED_DMP),
    bumpSelector(PICA_DATA_FRAG_LIGHT_ENV_TEXTURE0),
    isEnableShadowPrimary(false),
    isEnableShadowSecondary(false),
    isEnableShadowAlpha(false),
    isInvertShadow(false),
    isEnableBumpRenorm(false),
    isEnableClampHighLights(true),
    isEnableLutD0(false),
    isEnableLutD1(false),
    isEnableLutRefl(true){
    for (int i = 0; i < LIGHT_SOURCE_MAX; ++i){
        isEnable[i]  = false;
        isEnableSpot[i] = false;
        isEnableDistAttn[i] = false;
        isShadowed[i] = false;
        source[i].id = i;
    }
}

}
}
}
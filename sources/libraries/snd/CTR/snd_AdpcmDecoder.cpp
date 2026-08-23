// Filename: snd_AdpcmDecoder.cpp
//
// Project: Horizon

#include <nn/snd.h>

namespace nn { 
namespace snd { 
namespace CTR {

void DecodeAdpcmData(const u8* pInput, s16* pOutput, const AdpcmParam& param, AdpcmContext& context, s32 nSamples){
    static const s32 nOrder = 2;
    static const s32 nShift = 11;
    static const s32 nScale = (1 << nShift);

    const s16* pCoef = reinterpret_cast<const s16*>(param.coef);
    s16 yn1 = context.yn1, yn2 = context.yn2;

    u8 ps;
    while (nSamples > 0){
        s16 nibble[NN_SND_ADPCM_DOL_DATA_NUM_IN_BLOCK];

        ps = *pInput++;
        s32 gain = (ps & 0x0F);
        s32 ci = ps >> 4;
        s16 coef0 = pCoef[ci * nOrder + 0];
        s16 coef1 = pCoef[ci * nOrder + 1];

        for (int i = 0; i < NN_SND_ADPCM_DOL_DATA_NUM_IN_BLOCK; i += 2){
            u8 tmp = *pInput++;
            nibble[i]     = (s16)(tmp >> 4);
            nibble[i + 1] = (s16)(tmp & 0x0F);
        }

        s32 len = NN_SND_ADPCM_DOL_DATA_NUM_IN_BLOCK;
        if (nSamples < len){
            len = nSamples;
        }
        nSamples -= len;
        for (int i = 0; i < len ; i++){
            s32 mac = ((s32)nibble[i] << 28) >> (28 - gain - nShift);
            mac += yn1 * (s32)coef0 + yn2 * (s32)coef1;


            mac += (nScale >> 1);
            mac >>= nShift;

            mac = mac > -32768 ? mac : -32768;
            mac = mac <  32767 ? mac :  32767;

            *pOutput++ = (s16)mac;
            yn2 = yn1;
            yn1 = (s16)mac;
        }
    }

    context.pred_scale = ps;
    context.yn1 = yn1;
    context.yn2 = yn2;
}

}
}
}
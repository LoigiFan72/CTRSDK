#pragma once

#include "nn/dsp/CTR/Common/dsp_Types.h"

#define NN_SND_ADPCM_DOL_PS_SIZE_IN_NIB    (2)
#define NN_SND_ADPCM_DOL_DATA_NUM_IN_BLOCK (14)
#define NN_SND_ADPCM_DOL_NIBL_NUM_IN_BLOCK (16)

#ifdef NN_PROCESSOR_ARM11MPCORE
#define NN_SND_ADPCM_DOL_BLOCK_SIZE (8)
#else
#define NN_SND_ADPCM_DOL_BLOCK_SIZE (4)
#endif

typedef struct
{
    s16 a[8][2];
    u16 gain;
    u16 pred_scale;
    s16 yn1;
    s16 yn2;
    u16 loop_pred_scale;
    s16 loop_yn1;
    s16 loop_yn2;
    u16 pad[1];
} DspsndAdpcmState;

typedef struct 
{
    u32 num_samples;
    u32 num_adpcm_nibbles;
    u32 sample_rate;
    u16 loop_flag;
    u16 format;
    u32 sa;
    u32 ea;
    u32 ca;
    DspsndAdpcmState state;
    u16 pad[10];
} DspsndAdpcmHeader;

namespace nn {
namespace snd {
namespace CTR {

struct AdpcmParam 
{
    u16 coef[16];
};

struct AdpcmContext
 {
    u16 pred_scale;
    s16 yn1;
    s16 yn2;
};

} // namespace CTR
} // namespace snd
} // namespace nn
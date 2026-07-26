#pragma once

#include <nn/snd/CTR/MPCore/snd_DspFxReverb.h>
#include <nn/snd/CTR/MPCore/snd_DspFxDelay.h>

namespace nn{
namespace snd{
namespace CTR{
    struct DspsndAudioInfo{
        u8 nChannels;
        u8 format;
        u8 isFrontBypass;
        u8 isStartFrameFadeIn;
        u8 is3dSoundPreprocessed;
    };

    struct DspsndChannelPlayVars{
        u8 playState;
        u8 isBufJumped;
        short syncCount;
        u32 plypos;
        ushort currentBufferId;
        ushort lastBufferId;
    };

    struct DspsndChannelStatus{
        DspsndChannelPlayVars play_vars_rps;
    };

    struct DspsndDmaTransInfo{
        u32 srcIniAddress;
        u32 srcIniPlayLen;
        DspsndAudioInfo audioInfo;
        AdpcmContext adpcmContext;
        u8 isAdpcmContextUpdated;
        u8 loopFlag;
        ushort rev;
        ushort bufferId;
    };

    struct DspsndDspCycles0{
        s32 frame;
        s32 btdmp;
        s32 whole;
        s32 param;
        s32 sound;
    };

    struct DspsndDspCycles1{
        s32 voice[24];
        s32 timer[24];
        s32 align[24];
        s32 resample[24];
        s32 filter[24];
        s32 volume[24];
        s32 mix;
        s32 clip;
        s32 aux;
    };
 
    struct DspsndDspCycles{
        DspsndDspCycles0 ch0;
        DspsndDspCycles1 ch1;
    };

    struct DspsndMasterPlayParam{
        BusVolume volMaster;
        BusVolume volAux[2];
        u16 outputBufferCount;
        u16 control;
        u16 compressorStage;
        u16 outputMode;
        u16 clippingMode;
        u16 isHeadsetConnected;
        u16 surroundDepth;
        u16 surroundSpPosMode;
        u16 surroundlirFlag;
        u16 rearRatio;
        u16 isFrontBypassAux[2];
        u16 isAuxEnabled[2];
        DspFxDelayParams fxDelayParams[2];
        DspFxReverbParams fxReverbParams[2];
        u16 syncMode;
        u16 padding;
    };

    struct DspsndMasterDirect{
        u32 ctrl;
        DspsndMasterPlayParam param;
        u32 ctrl2;
    };

    struct DspsndMasterStatus{
        ushort control;
        ushort droppedFrameCount;
        ushort rev[14];
    };

    struct DspsndChannelOption{
        AdpcmParam adpcm_param;
    };
    
    struct DspsndNextBuffer{
        u32 bufAddress;
        s32 bufLength;
        AdpcmContext adpcmContext;
        u8 isUpdateContext;
        u8 loopFlag;
        u16 bufferId;
        u16 padding;
    };

    struct DspsndChannelPlayParam{
        f32 vol[3][4];
        f32 timer;
        u8 rimSelect;
        u8 coefSelect;
        s16 iir_type;
        s16 iir_coef_mono[2];
        s16 iir_coef_biqd[5];
        u16 nextBufCount;
        DspsndNextBuffer nextBuffer[4];
        u32 srcRepStartSmpl;
    };

    struct DspsndChannelDirect{
        u32 ctrl;
        DspsndChannelPlayParam play_param;
        DspsndChannelPlayVars play_vars;
        DspsndDmaTransInfo src_inf;
    };

    struct SndCommand{
        ushort command;
        ushort hole;
    };

}
}
}
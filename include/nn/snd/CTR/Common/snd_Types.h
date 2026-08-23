#pragma once

#include <nn/snd/CTR/Common/snd_Bcwav.h>
#include <string.h>

namespace nn {
namespace snd {
namespace CTR {

typedef struct{
    s16 n0;
    s16 d1;
} MonoFilterCoefficients;

typedef struct{
    s16 n0;
    s16 n1;
    s16 n2;
    s16 d1;
    s16 d2;
} BiquadFilterCoefficients;

typedef enum{
    CHANNEL_INDEX_FRONT_LEFT  = 0,
    CHANNEL_INDEX_FRONT_RIGHT = 1,
    CHANNEL_INDEX_REAR_LEFT   = 2,
    CHANNEL_INDEX_REAR_RIGHT  = 3,
    CHANNEL_INDEX_NUM         = 4
} ChannelIndex;

typedef enum{
    OUTPUT_MODE_MONO       = 0,
    OUTPUT_MODE_STEREO     = 1,
    OUTPUT_MODE_3DSURROUND = 2
} OutputMode;

typedef enum{
    CLIPPING_MODE_NORMAL = 0,
    CLIPPING_MODE_SOFT   = 1
} ClippingMode;

typedef enum{
    SURROUND_SPEAKER_POSITION_SQUARE = 0,
    SURROUND_SPEAKER_POSITION_WIDE   = 1,
    SURROUND_SPEAKER_POSITION_NUM
} SurroundSpeakerPosition;

enum InterpolationType{
    INTERPOLATION_TYPE_POLYPHASE,
    INTERPOLATION_TYPE_LINEAR,
    INTERPOLATION_TYPE_NONE
};

typedef enum{
    FILTER_TYPE_NONE     = 0,
    FILTER_TYPE_MONOPOLE = 1,
    FILTER_TYPE_BIQUAD   = 2,
    FILTER_TYPE_BOTH     = 3
} FilterType;

typedef enum{
    AUX_BUS_NULL = -1,
    AUX_BUS_A    =  0,
    AUX_BUS_B    =  1,
    AUX_BUS_NUM  =  2
} AuxBusId;

struct MixParam{
    f32 mainBus[CHANNEL_INDEX_NUM];
    f32 auxBusA[CHANNEL_INDEX_NUM];
    f32 auxBusB[CHANNEL_INDEX_NUM];

    MixParam(){
        for (int i = 0; i < CHANNEL_INDEX_NUM; i++){
            mainBus[i] = auxBusA[i] = auxBusB[i] = 0.0f;
        }
    }
};

struct WaveBuffer{
    enum Status{
        STATUS_FREE,
        STATUS_WAIT,
        STATUS_PLAY,
        STATUS_DONE,
        STATUS_TO_BE_DELETED
    };

    const void* bufferAddress;
    s32 sampleLength;
    const AdpcmContext* pAdpcmContext;
    uptr userParam;
    util::SizedEnum1<Status> status;
    s8 status;
    u16 bufferId;
    WaveBuffer* next;
};

enum SampleFormat{
    SAMPLE_FORMAT_PCM8  = Bcwav::ENCODING_PCM8,
    SAMPLE_FORMAT_PCM16 = Bcwav::ENCODING_PCM16,
    SAMPLE_FORMAT_ADPCM = Bcwav::ENCODING_DSP_ADPCM
};

static const s32 VOICE_PRIORITY_NODROP = 0x7fff;

typedef struct{
    s32* frontLeft;
    s32* frontRight;
    s32* rearLeft;
    s32* rearRight;
} AuxBusData;

typedef void(*AuxCallback)(AuxBusData* data, s32 sampleLength, uptr userData);

struct ThreadParameter{
    uptr   stackBuffer;
    uptr   stackSize;
    size_t priority;
};

union BusVolume{
    f32 f32value;
    short s16value;
};

struct OutputCapture{
    uptr mBufferAddess;
    s32 mBufferLength;
    s32 mReadPos;
    s32 mWritePos;
    bool mIsEnabled;
    u8 rev[3];

    void Write(short* pData, s32 length){
        memcpy((&this->mBufferAddess + this->mWritePos * 4), pData, length << 2);
        this->mWritePos += length;
        if(this->mBufferLength <= this->mWritePos)
            this->mWritePos = 0;
    }
};

enum SyncMode{
    SYNC_MODE_STRICT = 0,
    SYNC_MODE_LOOSE  = 1,
    SYNC_MODE_NUM    = 2
};

struct ThreadParameter{
    uptr   stackBuffer;
    uptr   stackSize;
    size_t priority;
};

} // namespace CTR
} // namespace snd
} // namespace nn

typedef enum nnsndChannelIndex{
    NN_SND_CHANNEL_INDEX_FRONT_LEFT,
    NN_SND_CHANNEL_INDEX_FRONT_RIGHT,
    NN_SND_CHANNEL_INDEX_REAR_LEFT,
    NN_SND_CHANNEL_INDEX_REAR_RIGHT,
    NN_SND_CHANNEL_INDEX_NUM
} nnsndChannelIndex;
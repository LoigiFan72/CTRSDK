#pragma once

#include "nn/snd/CTR/Common/snd_Adpcm.h"

namespace nn {
namespace snd {
namespace CTR {

class Bcwav {
private:
    Bcwav() {}
    ~Bcwav() {}

public:
    struct Reference 
    {
        u16 typeId;
        u16 padding;
        u32 offset;
    };

    struct ReferenceWithSize : public Reference 
    {
        u32 size;
    };

    template<typename ITEM_TYPE>
    struct Table 
    {
        u32 count;
        ITEM_TYPE item[1];
    };

    struct FileHeader 
    {
        u32 signature;
        u16 byteOrderMark;
        u16 headerSize;
        u32 version;
        u32 fileSize;
        u16 dataBlocks;
        u16 reserved;
    };

    struct BlockInfo 
    {
        ReferenceWithSize infoBlockReference;
        ReferenceWithSize dataBlockReference;
    };

    struct FileInfo 
    {
        FileHeader header;
        BlockInfo  blockInfo;
    };

    struct BlockHeader 
    {
        u32 kind;
        u32 size;
    };

    struct WaveInfo 
    {
        u8  encoding;
        u8  isLoop;
        u16 padding;
        u32 sampleRate;
        u32 loopStartFrame;
        u32 loopEndFrame;
    };

    struct InfoBlockBody 
    {
        WaveInfo         waveInfo;
        u32              reserved;
        Table<Reference> channelInfoReferenceTable;
    };

    struct ChannelInfo 
    {
        Reference toSamples;
        Reference toAdpcmInfo;
        u32       reserved;
    };

    struct DspAdpcmInfo 
    {
        AdpcmParam   param;
        AdpcmContext context;
        AdpcmContext loopContext;
    };

    struct ImaAdpcmContext 
    {
        s16 data;
        u8  tableIndex;
        u8  padding;
    };

    struct ImaAdpcmInfo 
    {
        ImaAdpcmContext context;
        ImaAdpcmContext loopContext;
    };

    struct InfoBlock 
    {
        BlockHeader   header;
        InfoBlockBody body;
    };

    typedef enum 
    {
        ENCODING_PCM8      = 0,
        ENCODING_PCM16     = 1,
        ENCODING_DSP_ADPCM = 2,
        ENCODING_IMA_ADPCM = 3,
        ENCODING_NUM       = 4
    } Encoding;

    typedef enum 
    {
        CHANNEL_INDEX_L = 0,
        CHANNEL_INDEX_R = 1
    } ChannelIndex;

    static const WaveInfo&        GetWaveInfo(const void* bcwav);
    static int                    GetChannelCount(const void* bcwav);
    static const void*            GetWave(const void* bcwav, int channelNo);
    static const DspAdpcmInfo*    GetDspAdpcmInfo(const void* bcwav, int channelNo);
    static const ImaAdpcmContext* GetImaAdpcmContext(const void* bcwav, int channelNo);
    static const ImaAdpcmContext* GetImaAdpcmLoopContext(const void* bcwav, int channelNo);
    static u32                    FrameToByte(u8 encoding, u32 frame);
    static const void*            AddOffsetToPtr(const void* ptr, int offset);
    static bool                   IsBcwav(const void* bcwav);
};

} // namespace CTR
} // namespace snd
} // namespace nn
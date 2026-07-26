#pragma once

#include <nn/gr/CTR/gr_BindSymbol.h>

namespace nn{
namespace gr{
namespace CTR{
    
class Vertex{
public:
    static const u32 VERTEX_ATTRIBUTE_MAX = 12;
    static const u32 VERTEX_ATTRIBUTE_DIMENSION_MAX = 4;
    static const u32 VERTEX_ENABLE_COMMAND_MAX = 12 + VERTEX_ATTRIBUTE_MAX * 6;

    class InterleaveInfo{
    public:
        InterleaveInfo(){
            dataNum = 0;

            for (u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++){
                dataType[index] = PICA_DATA_SIZE_1_BYTE;
            }

            for (u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++){
                symbol[index] = NULL;
            }
        }

    public:
        u8 dataNum;
        s8 rev[3];
        PicaDataVertexAttrType dataType[VERTEX_ATTRIBUTE_MAX];
        const nn::gr::CTR::BindSymbolVSInput* symbol[VERTEX_ATTRIBUTE_MAX];    
    };

    void DisableAll(){
        mCmdCacheVertexNum = 0;

        for (u32 index = 0; index < VERTEX_ENABLE_COMMAND_MAX; index++){
                mCmdCacheVertex[index] = 0;
            }

        for (u32 index = 0; index < 12; index++){
            mIsEnableReg[index] = false;
            this->mLoadArray[index].DisableAll();
            this->mAttrConst[index].DisableAll();
        }
    }
    void EnableInterleavedArray(const nn::gr::CTR::Vertex::InterleaveInfo& interleave_info, const uptr physical_addr);
public:
    explicit Vertex(){
        this->DisableAll();
    }

protected:
    class LoadArray{
    public:
        LoadArray():
            physicalAddr(0)
        {}

        uptr physicalAddr;
        PicaDataVertexAttrType type[12];
        u32 byte[12];
        s32 bind[12];

        bool IsEnable() const { return physicalAddr != 0; }

        void DisableAll();
    };
    class AttrConst{
    public:
        AttrConst():
            dimension(0)
        {}

        u8 dimension;
        s8 rev[3];
        f32 param[VERTEX_ATTRIBUTE_DIMENSION_MAX];

        bool IsEnable() const { return dimension != 0; }

        void DisableAll(){
            dimension = 0;

            for (u32 index = 0; index < VERTEX_ATTRIBUTE_DIMENSION_MAX; index++){
                param[index] = 0;
            }
        }
    };
    
    mutable u32 mCmdCacheVertexNum;
    mutable bit32 mCmdCacheVertex[VERTEX_ENABLE_COMMAND_MAX];
    bool mIsEnableReg[VERTEX_ATTRIBUTE_MAX];

    LoadArray mLoadArray[VERTEX_ATTRIBUTE_MAX];
    AttrConst mAttrConst[VERTEX_ATTRIBUTE_MAX];
};
}
}
}
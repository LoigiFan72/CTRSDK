#pragma once

#include <nn/gr/CTR/gr_BindSymbol.h>

namespace nn{
namespace gr{
namespace CTR{
    
class Vertex
{
public:
    static const u32 VERTEX_ATTRIBUTE_MAX = 12;
    static const u32 VERTEX_ATTRIBUTE_DIMENSION_MAX = 4;
    static const u32 VERTEX_ENABLE_COMMAND_MAX = 12 + VERTEX_ATTRIBUTE_MAX * 6;

    class InterleaveInfo
    {
    public:
        InterleaveInfo()
        {
            dataNum = 0;

            for (u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++)
            {
                dataType[index] = PICA_DATA_SIZE_1_BYTE;
            }

            for (u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; index++)
            {
                symbol[index] = NULL;
            }
        }

    public:
        u8 dataNum;
        s8 rev[3];
        PicaDataVertexAttrType dataType[VERTEX_ATTRIBUTE_MAX];
        const nn::gr::CTR::BindSymbolVSInput* symbol[VERTEX_ATTRIBUTE_MAX];    
    };

    class IndexStream
    {
    public:
        IndexStream():
            physicalAddr(0),
            drawVtxNum(0),
            isUnsignedByte(false)
        {
        }

        uptr  physicalAddr;
        u32  drawVtxNum;
        bool isUnsignedByte;
        u32 pad;
    };

    static bit32* MakeDisableCommand(bit32* command)
    {
        *command++ = 0;
        *command++ = PICA_CMD_HEADER_SINGLE(0x201);

        *command++ = 0;
        *command++ = PICA_CMD_HEADER_SINGLE(0x202);

        const int size = (2 + 3 * VERTEX_ATTRIBUTE_MAX) * sizeof(bit32);
        std::memset(command, 0, size);
        command[1] = PICA_CMD_HEADER_BURSTSEQ(PICA_REG_LOAD_ARRAY0_ATTR_OFFSET, VERTEX_ATTRIBUTE_MAX * 3);
        command += size / sizeof(bit32);
                    
        for (int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i)
        {
            *command++ = i;
            *command++ = PICA_CMD_HEADER_BURSTSEQ(PICA_REG_VS_FIXED_ATTR, 4);

            *command++ = 0;
            *command++ = 0;

            *command++ = 0;
            *command++ = 0;
        }

        return command;
    }

    void DisableAll()
    {
        m_CmdCacheVertexNum = 0;

        for (u32 index = 0; index < VERTEX_ENABLE_COMMAND_MAX; index++)
        {
            m_CmdCacheVertex[index] = 0;
        }

        for (u32 index = 0; index < 12; index++)
        {
            m_IsEnableReg[index] = false;
            m_LoadArray[index].DisableAll();
            m_AttrConst[index].DisableAll();
        }
    }
    void EnableInterleavedArray(const nn::gr::CTR::Vertex::InterleaveInfo& interleave_info, const uptr physical_addr);
    void EnableAttrAsArray(const nn::gr::CTR::BindSymbolVSInput& symbol, const uptr physical_addr, const PicaDataVertexAttrType type);
private:
    void DisableAttr_(const bit32 bind_reg);
public:
    explicit Vertex()
    {
        this->DisableAll();
    }
protected:
    class LoadArray
    {
    public:
        LoadArray():
            physicalAddr(0)
        {
        }

        uptr physicalAddr;
        PicaDataVertexAttrType type[12];
        u32 byte[12];
        s32 bind[12];

        bool IsEnable() const { return physicalAddr != 0; }
        void DisableAll();
        void CheckDisable();
    };
    class AttrConst
    {
    public:
        AttrConst():
            dimension(0)
        {
        }

        u8 dimension;
        s8 rev[3];
        f32 param[VERTEX_ATTRIBUTE_DIMENSION_MAX];

        bool IsEnable() const { return dimension != 0; }
        void Disable(){ dimension = 0; }

        void DisableAll()
        {
            dimension = 0;

            for (u32 index = 0; index < VERTEX_ATTRIBUTE_DIMENSION_MAX; index++)
            {
                param[index] = 0;
            }
        }
    };
    
    mutable u32 m_CmdCacheVertexNum;
    mutable bit32 m_CmdCacheVertex[VERTEX_ENABLE_COMMAND_MAX];
    bool m_IsEnableReg[VERTEX_ATTRIBUTE_MAX];

    LoadArray m_LoadArray[VERTEX_ATTRIBUTE_MAX];
    AttrConst m_AttrConst[VERTEX_ATTRIBUTE_MAX];
};
}
}
}
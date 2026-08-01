#pragma once

#include <nn/gr/CTR/gr_Utility.h>
#include <nn/gr/CTR/gr_BindSymbol.h>
#include <string>

namespace nn{
namespace gr{
namespace CTR{

class Shader{
public:
    Shader();
    void SetupBinary(const void* shader_binary, const s32 vtx_shader_index, const s32 geo_shader_index);
public:
    void SetDrawMode(const PicaDataDrawMode draw_mode){
        mDrawMode = draw_mode;
    }

public:
    bit32* MakeFullCommand(bit32* command) const;
    bit32* MakeShaderCommand(bit32* command, const bool isMakePrepareCommand = true) const;
    static bit32* MakeDisableCommand(bit32* command);

public:
    bit32* MakeVtxBoolMapCommand(bit32* command) const{
        *command++ = 0x7fff0000 | mVtxShaderBoolMapUniform;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_VS_BOOL);
        return command;
    }

    bit32* MakeGeoBoolMapCommand(bit32* command) const{
        *command++ = 0x7fff0000 | mGeoShaderBoolMapUniform;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_GS_BOOL);
        return command;
    }
                
    bit32* MakePrepareCommand(bit32* command) const;
    bit32* MakeVtxProgramCommand(bit32* command) const;
    bit32* MakeGeoProgramCommand(bit32* command) const;

    bit32* MakeVtxSwizzleCommand(bit32* command) const{
        *command++ = 0;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_VS_PROG_SWIZZLE_ADDR);
        return this->MakeLoadCommand_(command, PICA_REG_VS_PROG_SWIZZLE_DATA0, mSwizzle, mSwizzleCount);
    }

    bit32* MakeGeoSwizzleCommand(bit32* command) const{
        *command++ = 0;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_GS_PROG_SWIZZLE_ADDR);
        return this->MakeLoadCommand_(command, PICA_REG_GS_PROG_SWIZZLE_DATA0, mSwizzle, mSwizzleCount);
    }


    bit32* MakeVtxConstRgCommand(bit32* command) const{
        std::memcpy(command, mCmdCacheConstArray[mVtxShaderIndex],mCmdCacheConstNumArray[mVtxShaderIndex] * sizeof(bit32));
        return command + mCmdCacheConstNumArray[mVtxShaderIndex];
    }

    bit32* MakeGeoConstRgCommand(bit32* command) const{
        std::memcpy(command, mCmdCacheConstArray[mGeoShaderIndex], mCmdCacheConstNumArray[mGeoShaderIndex] * sizeof(bit32));        
        return command + mCmdCacheConstNumArray[ mGeoShaderIndex ];
    }

    bit32* MakeOutAttrCommand(bit32* command) const{
        NN_ASSERT_((!this->IsEnableGeoShader()) || (mDrawMode == PICA_DATA_DRAW_GEOMETRY_PRIMITIVE));

        bit32 value_229 = 0x0100;
        if (mDrawMode != PICA_DATA_DRAW_TRIANGLES ){
            value_229 = 0x0;
        }

        *command++ = value_229;
        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE0, 0x2 );

        bit32 value_253 = 0x0100;
        if (mDrawMode != PICA_DATA_DRAW_TRIANGLES){
            value_253 = 0x0;
        }
        *command++ = value_253;
        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE1, 0x2 );

        std::memcpy(command, mCmdCacheOutAttrArray,mCmdCacheOutAttrNum * sizeof(bit32));
        return command + mCmdCacheOutAttrNum;
    }

public:
    bool SearchBindSymbol(BindSymbol* symbol,const char* name) const;
    u32 SearchBindSymbolNum(const BindSymbol::ShaderType shader_type,const BindSymbol::SymbolType symbol_type) const;               
    bool SearchBindSymbol(BindSymbol* symbol,const u8 symbol_index) const;
    void SetShaderIndex(s32 vtx_shader, s32 geo_shader);

    u8 GetShaderNum() const{ return mExeImageInfoNum; }
    bool IsEnableGeoShader() const{ return (0 <= this->GetGeoShaderIndex()); }
    s32 GetVtxShaderIndex() const{ return mVtxShaderIndex; }              
    s32 GetGeoShaderIndex() const{ return mGeoShaderIndex; }
    bit32 GetVtxShaderBoolMapUniform() const{ return mVtxShaderBoolMapUniform; }
    bit32 GetGeoShaderBoolMap() const{ return mGeoShaderBoolMapUniform; }
                
protected:
    static const s32 EXE_IMAGE_MAX = 32;
    static const s32 SWIZZLE_PATTERN_MAX = 128;
    static const s32 DUMMY_DATA_NUM_251 = 10;
    static const s32 DUMMY_DATA_NUM_200 = 30;
    static const u32 PADDING_DATA = 0xead0fead;
    static const s32 OUT_ATTR_COMMAND_MAX = 48;
    static const u32 CONST_REG_COMMAND_MAX = 96 * 6 + 5 * 2;

protected:
    void CheckVtxShaderIndex_(const s32 vtx_shader_index);
    void CheckGeoShaderIndex_(const s32 geo_shader_index);
    void MakeShaderConstCommandCache_();
    void MakeShaderOutAttrCommandCache_();
    static bit32* MakeShaderModeCommand_(bit32* command,const bool isEnableGeoShader,const PicaDataDrawMode drawMode);
    bit32* MakeConstRgCommand_(bit32* command,const s32 shader_index);
    bit32* MakeOutAttrCommand_(bit32* command, const s32 vtx_shader_index, const s32 geo_shader_index);
    bit32* MakeLoadCommand_(bit32* command,const bit32  load_reg,const u32* src_buffer_ptr,const u32 src_size) const;
    static bit32* MakeDummyCommand_(bit32* command,const bit32 load_reg,const u32 data_num);

    struct ExeImageInfo{
        bit32 signature;
        u16 version;
        u8  isGeoShader;
        u8  outputMaps;
        uptr mainAddress;
        uptr endAddress;
        bit32 maskInputOutput;
        u8  gsDataMode;
        u8  gsVertexStartIndex;
        u8  gsPatchSize;
        u8  gsVertexNum;
        u32 setupOffset;
        u32 setupCount;
        u32 labelOffset;
        u32 labelCount;
        u32 outMapOffset;
        u32 outMapCount;
        u32 bindSymbolOffset;
        u32 bindSymbolCount;
        u32 stringOffset;
        u32 stringCount;
    };


    s32 mVtxShaderIndex;
    s32 mGeoShaderIndex;
    u8 mExeImageInfoNum;
    s8 rev1[3];
    const ExeImageInfo* mExeImageInfo[EXE_IMAGE_MAX];
    const bit32* mInstruction;
    u32 mInstructionCount;
    bit32 mSwizzle[SWIZZLE_PATTERN_MAX];
    u32 mSwizzleCount;
    PicaDataDrawMode mDrawMode;
    s8 rev2[3];
    bit32 mVtxShaderBoolMapUniform;
    bit32 mGeoShaderBoolMapUniform;
    bit32 mCmdCacheOutAttrArray[OUT_ATTR_COMMAND_MAX];
    u32 mCmdCacheOutAttrNum;
    bit32 mCmdCacheConstArray[EXE_IMAGE_MAX][CONST_REG_COMMAND_MAX];
    u32 mCmdCacheConstNumArray[EXE_IMAGE_MAX];
};

}
}
}
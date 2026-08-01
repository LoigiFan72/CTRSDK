// Filename: gr_Shader.cpp
//
// Project: Horizon

#include <nn/gr/CTR/gr_Shader.h>

namespace nn{
namespace gr{
namespace CTR{

Shader::Shader():
    mVtxShaderIndex(0),
    mGeoShaderIndex(-1),
    mExeImageInfoNum(0),
    mInstructionCount(0),
    mSwizzleCount(0),
    mDrawMode(PICA_DATA_DRAW_TRIANGLES),
    mVtxShaderBoolMapUniform(0),
    mGeoShaderBoolMapUniform(0),
    mCmdCacheOutAttrNum(0){
    for (s32 shader_index = 0; shader_index < EXE_IMAGE_MAX; shader_index++){
        mCmdCacheConstNumArray[shader_index] = 0;
    }
}

void Shader::SetupBinary(const void* shader_binary, const s32 vtx_shader_index, const s32 geo_shader_index){
    const bit32* binary = reinterpret_cast< const bit32* >( shader_binary );
    NN_ASSERT_(binary != NULL );

    NN_ASSERT_(*binary == 0x424C5644);
    ++binary;

    NN_ASSERT_(*binary < EXE_IMAGE_MAX);

    mExeImageInfoNum = *binary;
    ++binary;

    mVtxShaderBoolMapUniform = 0;
    mGeoShaderBoolMapUniform = 0;

    for (s32 i = 0; i < mExeImageInfoNum; ++i){
        mExeImageInfo[i] = reinterpret_cast< const ExeImageInfo* >( (u8*)shader_binary + *binary );
        NN_ASSERT_(mExeImageInfo[i]->signature == 0x454c5644);
        ++binary;
    }
                
    const bit32* package_info = binary;
    NN_ASSERT_(*binary ==0x504C5644);
    ++binary;
    ++binary;

    mInstruction = reinterpret_cast< const bit32* >( (u8*)package_info + *binary );
    ++binary;

    mInstructionCount = *binary;
    ++binary;

    const bit32* swizzle = reinterpret_cast< const bit32* >( (u8*)package_info + *binary );
    ++binary;

    mSwizzleCount = *binary;
    NN_ASSERT_(mSwizzleCount < SWIZZLE_PATTERN_MAX);
    ++binary;

    for (s32 i = 0; i < mSwizzleCount; i++){
        mSwizzle[i] = swizzle[i * 2];
    }

    PicaDataDrawMode drawMode  = mDrawMode;

    this->MakeShaderConstCommandCache_();
    this->SetShaderIndex(vtx_shader_index, geo_shader_index);
                
    if (!this->IsEnableGeoShader() ){
        mDrawMode = drawMode;
    }
}

void Shader::SetShaderIndex(const s32 vtx_shader_index, const s32 geo_shader_index){
    this->CheckVtxShaderIndex_(vtx_shader_index);
    this->CheckGeoShaderIndex_(geo_shader_index);
                
    mVtxShaderIndex = vtx_shader_index;
    mGeoShaderIndex = geo_shader_index;

    if(this->IsEnableGeoShader()){
        mDrawMode = PICA_DATA_DRAW_GEOMETRY_PRIMITIVE;
    }
               
    this->MakeShaderOutAttrCommandCache_();
}

void Shader::MakeShaderConstCommandCache_(){
    for (s32 shader_index = 0; shader_index < mExeImageInfoNum; shader_index++){
        mCmdCacheConstNumArray[shader_index] = this->MakeConstRgCommand_(mCmdCacheConstArray[shader_index], shader_index) - mCmdCacheConstArray[shader_index];
        NN_ASSERT_(mCmdCacheConstNumArray[shader_index] <= CONST_REG_COMMAND_MAX);
    }
}

void Shader::MakeShaderOutAttrCommandCache_(){
    mCmdCacheOutAttrNum = this->MakeOutAttrCommand_(mCmdCacheOutAttrArray,mVtxShaderIndex,mGeoShaderIndex)- mCmdCacheOutAttrArray;
}

bit32* Shader::MakeFullCommand(bit32* command) const{
    {
        command = MakePrepareCommand(command);
    }

    if (this->IsEnableGeoShader()){                     
        command = this->MakeGeoProgramCommand(command);
        command = this->MakeGeoSwizzleCommand(command);
        command = this->MakeGeoConstRgCommand(command);
        command = this->MakeGeoBoolMapCommand(command);
    }            

    {
        command = this->MakeVtxProgramCommand(command);
        command = this->MakeVtxSwizzleCommand(command);
        command = this->MakeVtxConstRgCommand(command);
        command = this->MakeVtxBoolMapCommand(command);
    }

    {
        command = this->MakeOutAttrCommand(command);
    }
                
    return command;
}

bit32* Shader::MakeDisableCommand(bit32* command){
    const bool isEnableGeometryShader = false;
    const PicaDataDrawMode drawMode = PICA_DATA_DRAW_TRIANGLES;

    command = MakeShaderModeCommand_(command,isEnableGeometryShader,drawMode);

    return command;
}

bit32* Shader::MakeShaderCommand(bit32* command, const bool isMakePrepareCommand) const{
    if (isMakePrepareCommand){
        command = this->MakePrepareCommand(command);
    }

    if (this->IsEnableGeoShader()){
        command = this->MakeGeoConstRgCommand(command);
        command = this->MakeGeoBoolMapCommand(command);
    }

    {
        command = this->MakeVtxConstRgCommand(command);
        command = this->MakeVtxBoolMapCommand(command);
    }

    {
        command = this->MakeOutAttrCommand(command);
    }

    return command;
}

bit32* Shader::MakePrepareCommand(bit32* command) const{
    bool isEnableGeoShader = this->IsEnableGeoShader();
    PicaDataDrawMode drawMode = mDrawMode;

    command = this->MakeShaderModeCommand_(command, isEnableGeoShader, drawMode);

    return command;
}

bit32* Shader::MakeVtxProgramCommand(bit32* command) const{
    s32 shader_index   = this->GetVtxShaderIndex();
    bit32 reg_addr     = PICA_REG_VS_PROG_ADDR;
    bit32 reg_load     = PICA_REG_VS_PROG_DATA0;
    bit32 reg_end      = PICA_REG_VS_PROG_UPDATE_END;

    {
        *command++ = 0;
        *command++ = PICA_CMD_HEADER_SINGLE(reg_addr);
    }

    {
        NN_ASSERT_(0 <= shader_index && shader_index < mExeImageInfoNum);
                    
        const ExeImageInfo* exe_info = mExeImageInfo[shader_index];

        u32 instructionCount = mInstructionCount;
        if (instructionCount > 512){
            instructionCount = 512;
        }

        command = this->MakeLoadCommand_(command, reg_load, mInstruction, mInstructionCount < 512 ? mInstructionCount : 512);
    }

    {
        *command++ = 1;
        *command++ = PICA_CMD_HEADER_SINGLE(reg_end);
    }

    return command;
}

bit32* Shader::MakeGeoProgramCommand( bit32* command ) const{
    s32 shader_index   = this->GetGeoShaderIndex();
    bit32 reg_addr     = PICA_REG_GS_PROG_ADDR;
    bit32 reg_load     = PICA_REG_GS_PROG_DATA0;
    bit32 reg_end      = PICA_REG_GS_PROG_UPDATE_END;

    {
        *command++ = 0;
        *command++ = PICA_CMD_HEADER_SINGLE(reg_addr);
    }

    {
        NN_ASSERT_((0 <= shader_index) && (shader_index < mExeImageInfoNum));
                        
        const ExeImageInfo* exe_info = mExeImageInfo[shader_index];

        NN_UNUSED_VAR(exe_info);

        command = this->MakeLoadCommand_(command, reg_load, this->mInstruction, this->mInstructionCount);
    }

    {
        *command++ = 1;
        *command++ = PICA_CMD_HEADER_SINGLE( reg_end );
    }

    return command;
}

bit32* Shader::MakeShaderModeCommand_(bit32* command, const bool isEnableGeoShader, const PicaDataDrawMode drawMode){
    { 
        if (isEnableGeoShader){
            *command++ = PICA_DATA_DRAW_GEOMETRY_PRIMITIVE << 8;
        }
        else{
            *command++ = drawMode << 8;
        }

        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_VS_OUT_REG_NUM3, 2 );
    }

    {
        command = MakeDummyCommand_(command, PICA_REG_VS_OUT_REG_NUM2, DUMMY_DATA_NUM_251);
    }

    {
        command = MakeDummyCommand_(command, PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, DUMMY_DATA_NUM_200);
    }

    {
        *command++ = isEnableGeoShader ? 2 : 0;
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE0, 1);
    }

    {
        command = MakeDummyCommand_(command, PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, DUMMY_DATA_NUM_200);
    }

    { 
        *command++ = isEnableGeoShader ? 1 : 0;
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_COM_MODE, 1);
    }

    return command;
}

void Shader::CheckVtxShaderIndex_( const s32 vtx_shader_index ){
    NN_UNUSED_VAR(vtx_shader_index);

    NN_ASSERT_((0 <= vtx_shader_index) && (vtx_shader_index < this->GetShaderNum()));

    NN_ASSERT_(!mExeImageInfo[vtx_shader_index]->isGeoShader);
}

void Shader::CheckGeoShaderIndex_( const s32 geo_shader_index ){
    NN_UNUSED_VAR(geo_shader_index);

    NN_ASSERT_(mGeoShaderIndex < GetShaderNum());

    if (geo_shader_index > - 1){
        NN_ASSERT_(mExeImageInfo[geo_shader_index]->isGeoShader);
    }
}

bit32* Shader::MakeConstRgCommand_(bit32* command, const s32 shader_index){
    bit32  reg_float     = PICA_REG_VS_FLOAT_ADDR;
    bit32  reg_integer   = PICA_REG_VS_INT0;
    bit32* boolMap       = &mVtxShaderBoolMapUniform;

    bool is_geometry_shader = mExeImageInfo[shader_index]->isGeoShader;
    if (is_geometry_shader){
        reg_float = PICA_REG_GS_FLOAT_ADDR;
        reg_integer = PICA_REG_GS_INT0;
        boolMap = &mGeoShaderBoolMapUniform;
    }

    NN_ASSERT_((0 <= shader_index) && (shader_index < mExeImageInfoNum));
    const ExeImageInfo* exe_info = mExeImageInfo[shader_index];

    struct SetupInfo{
        u16 type;
        u16 index;
        bit32 value[4];
    };

    const SetupInfo* setupInfo = reinterpret_cast<const SetupInfo*>(reinterpret_cast<const u8*>(exe_info) + exe_info->setupOffset);

    for (int i = 0; i < exe_info->setupCount; ++i){
        const SetupInfo& info = setupInfo[i];
        const bit32* value = info.value;

        switch (info.type){
        case 0:
            *boolMap |= (info.value[0] << info.index) & (1 << info.index);
            break;
        case 1:
            *command++ = value[0] | value[1] <<  8 | value[2] << 16 | value[3] << 24;
            *command++ = PICA_CMD_HEADER_SINGLE(reg_integer + info.index);
             break;
        case 2:
            *command++ = info.index;
            *command++ = PICA_CMD_HEADER_BURSTSEQ( reg_float, 4 );
            *command++ = (value[3] <<  8 & 0xffffff00) | (value[2] >> 16 & 0x000000ff);
            *command++ = (value[2] << 16 & 0xffff0000) | (value[1] >>  8 & 0x0000ffff);
            *command++ = (value[1] << 24 & 0xff000000) | (value[0] >>  0 & 0x00ffffff);
            *command++ = PADDING_DATA;
            break;
        }
    }

    return command;
}


        
}
}
}
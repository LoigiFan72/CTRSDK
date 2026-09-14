#pragma once

#include <nn/gr/CTR/gr_Utility.h>

namespace nn{
namespace gr{
namespace CTR{

class BindSymbol
{
public:
    enum SymbolType
    {
        SYMBOL_TYPE_INVALID, 
        SYMBOL_TYPE_INPUT,   
        SYMBOL_TYPE_FLOAT,
        SYMBOL_TYPE_INTEGER, 
        SYMBOL_TYPE_BOOL
    };

    enum ShaderType
    {
        SHADER_TYPE_VERTEX,  
        SHADER_TYPE_GEOMETRY 
    };

    const ShaderType shaderType;
    const SymbolType symbolType;
    u8 start;
    u8 end;
    const char* name;
protected:
    explicit BindSymbol(const ShaderType shader_type, const SymbolType symbol_type): 
        shaderType(shader_type),
        symbolType(symbol_type),
        start(0xff),
        end(0xff),
        name(NULL)
    {
    }
};

class BindSymbolVSInput : public BindSymbol
{
public:
    explicit BindSymbolVSInput(): 
        BindSymbol(SHADER_TYPE_VERTEX, SYMBOL_TYPE_INPUT)
    {}
};

class BindSymbolVSFloat : public BindSymbol
{
public:
    explicit BindSymbolVSFloat(): 
        BindSymbol(SHADER_TYPE_VERTEX, SYMBOL_TYPE_FLOAT)
    {
    }

    bit32* MakeUniformCommand(bit32* command, const nn::math::MTX34& mtx34) const
    {
        return MakeUniformCommandVS(command, start, mtx34);
    }

    bit32* MakeUniformCommand(bit32* command, const nn::math::MTX44& mtx44) const
    {
        return MakeUniformCommandVS(command, start, mtx44);
    }

    bit32* MakeUniformCommand(bit32* command, const nn::math::VEC4& vec4) const
    {
        return MakeUniformCommandVS(command, start, vec4);
    }
};

class BindSymbolVSBool : public BindSymbol
{
public:
    explicit BindSymbolVSBool(): 
        BindSymbol(SHADER_TYPE_VERTEX, SYMBOL_TYPE_BOOL)
    {
    }
};

class BindSymbolGSFloat : public BindSymbol
{
public:
    explicit BindSymbolGSFloat(): 
        BindSymbol(SHADER_TYPE_GEOMETRY, SYMBOL_TYPE_FLOAT)
    {
    }

    bit32* MakeUniformCommand(bit32* command, const nn::math::VEC4& vec4) const
    {
        return MakeUniformCommandGS(command, start, vec4);
    }
};

}
}
}
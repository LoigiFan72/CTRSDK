#pragma once


#include <nn/gr/CTR/gr_Utility.h>

namespace nn{
namespace gr{        
namespace CTR{

class Texture{
public:

    class UnitBase{
    public:
        uptr physicalAddr;
        u16 width;
        u16 height;
        PicaDataTextureFormat format;
        PicaDataTextureWrap wrapT;
        PicaDataTextureWrap wrapS;
        PicaDataTextureMagFilter magFilter;       
        PicaDataTextureMinFilter minFilter;
        s8 rev[3];
        f32 lodBias;
        u8 minLodLevel;
        u8 maxLodLevel;
        u8 borderColorR;
        u8 borderColorG;
        u8 borderColorB;
        u8 borderColorA;
        s16 rev2;

    protected:
        bool IsEnableMipMap() const{
            if (minFilter == PICA_DATA_TEXTURE_MIN_FILTER_LINEAR){
                    return false;
                }

                if (minFilter == PICA_DATA_TEXTURE_MIN_FILTER_NEAREST){
                    return false;
                }

            return true;
        }

        UnitBase();
    };

    class Unit0 : public UnitBase{
    public:
        PicaDataTexture0SamplerType texType;
        s8 rev[3];
        uptr cubeMapAddrPosX;
        uptr cubeMapAddrNegX;
        uptr cubeMapAddrPosY;
        uptr cubeMapAddrNegY;
        uptr cubeMapAddrPosZ;
        uptr cubeMapAddrNegZ;

        bit32* MakeCommand(bit32* command, bool is_update_texture_func = true) const;
        explicit Unit0(const Texture& texture_);

    protected:
        const Texture& mTexture;
    };

    class Unit1 : public UnitBase{
    public:
        PicaDataTexture1SamplerType texType;
        s8 rev[3];

        bit32* MakeCommand(bit32* command, bool is_update_texture_func = true) const;
        explicit Unit1(const Texture& texture_);
                    
    protected:
        const Texture& mTexture;
    };

    class Unit2 : public UnitBase{
    public:
        PicaDataTexture2SamplerType texType;
        PicaDataTexture2TexCoord    texCoord;
        s16 rev;

        bit32* MakeCommand(bit32* command, bool is_update_texture_func = true) const;
        explicit Unit2(const Texture& texture_);

    protected:
        const Texture& mTexture;
    };

    class Unit3 : protected UnitBase{
    public:   
        PicaDataTexture3SamplerType texType;
        PicaDataTexture3TexCoord    texCoord;
        s16 rev;

        bit32* MakeCommand(bit32* command, bool is_update_texture_func = true) const;
        explicit Unit3(const Texture& texture_);

    protected:
        const Texture& mTexture;
    };

    Unit0 unit0;
    Unit1 unit1;
    Unit2 unit2;
    Unit3 unit3;

    Texture();
    bit32* MakeCommand(bit32* command, bool isAddDummyCommand = true) const;
    bit32* MakeFuncCommand(bit32* command, bool isAddDummyCommand = true) const;
    static bit32* MakeDisableCommand( bit32* command, bool isAddDummyCommand = true );
protected:
    static bit32* MakeDummyCommand_( bit32* command );
};

}
}
}
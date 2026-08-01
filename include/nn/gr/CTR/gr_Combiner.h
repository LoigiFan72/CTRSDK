#pragma once

#include <nn/gx.h>
#include <nn/gx/CTR/gx_CommandAccess.h>

namespace nn{
namespace gr{
namespace CTR{

class Combiner{
public:
    class Stage{
    public:
        struct CombineFunction{
            PicaDataTexEnvCombine combine;
            PicaDataTexEnvOperand operand[3];
            PicaDataTexEnvSrc source[3];
            PicaDataTexEnvScale scale;
            PicaDataTexEnvBufferInput bufferInput;
            explicit CombineFunction(const u8 stage_index, bool is_rgb);

        protected:
            friend class Stage;
            explicit CombineFunction(){};
        };

        CombineFunction rgb;
        CombineFunction alpha;
        u8 constColorR;
        u8 constColorG;
        u8 constColorB;
        u8 constColorA;
        PicaReg headRegister;

        public:
            explicit Stage(const int stage_index);
            void SetupPrimary();
            void SetupPrevious();
            void SetupTexture0();
            void SetupPrimaryModulateTexture0();

            bit32* MakeCommand(bit32* command) const;
            bit32* MakeConstantColorCommand(bit32* command) const{
                u32 constColorReg = headRegister + 3;
                                
                *command++ = PICA_CMD_DATA_TEX_ENV_CONST(constColorR, constColorG,constColorB, constColorA );      
                *command++ = PICA_CMD_HEADER_SINGLE( constColorReg );

                return command;
            }

        protected:
            friend class Combiner;
            explicit Stage(){};
        };
public :
    static const u32 COMBINER_STAGE_MAX = 6;
    Stage stage[COMBINER_STAGE_MAX];

public:
    u8 bufferColorR;
    u8 bufferColorG;
    u8 bufferColorB;
    u8 bufferColorA;

    explicit Combiner();
    bit32* MakeCommand(bit32* command) const;
    bit32* MakeCombinerBufferCommand(bit32* command) const;
};

}
}
}
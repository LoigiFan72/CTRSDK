#pragma once

#include <nn/math.h>

namespace nn{
namespace CTR{

inline bit32 MakeTitleCode(bit32 id, bit8 variation){ 
    return (id & 0xff0fffff) << 8 | variation; 
}

inline bit32 MakeProgramIdHi(bit32 ctg){ 
    return ctg | 0x400000; 
}

inline bit32 MakeProgramIdLo(bit32 id, bit8 variation){ 
    return MakeTitleCode(id, variation); 
}

inline ProgramId MakeProgramId(bit32 ctg,bit32 id,bit8 variation){ 
    return (MakeProgramIdHi(ctg), (MakeProgramIdLo(id, variation))); 
}

inline bit16 GetCategoryOf(ProgramId pgid){
    return math::GetBits<bit16>(pgid, 32, 16);
}

inline bool IsCtr(ProgramId pgid){
    return (static_cast<bit32>(pgid >> 32) & 0xFFFFC000) == 0x00040000;
}

inline bool IsAddOnContents(ProgramId pgid){
    return IsCtr(pgid) && GetCategoryOf(pgid) == 140;
}

}
}
#pragma once

#include <nn/types.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/ro/ro_ObjectFile.h>
#include <nn/ro/ro_Types.h>

namespace nn { 
namespace ro { 
namespace detail {

class SectionTable{
private:
    const SectionInfo*  mpSectionTable;
    s32 mNumSections;

public:
    SectionTable(const ModuleHeader* pModule): 
        mpSectionTable(pModule->sectionInfo), 
        mNumSections(pModule->numSections)
    {}

    uptr GetAddress(int index, u32 offset) const{
        if(!((0 <= index) && (index < mNumSections))){
            return NULL;
        }

        const SectionInfo& section = mpSectionTable[index];

        if(!(offset < section.size)){
            return NULL;
        }

        return section.offset + offset;
    }

    uptr GetAddress(SectionAndOffset sao) const{
        return GetAddress(sao.GetFirst(), sao.GetSecond());
    }


    template<typename T>
    T GetPointer(SectionAndOffset sao) const{
        return reinterpret_cast<T>(GetAddress(sao));
    }
};

}
}
}
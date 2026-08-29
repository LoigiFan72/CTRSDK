#pragma once

#include <nn/Handle.h>
#include <nn/dev/CTR/dev_Api.h>
#include <string.h>

namespace nn{
namespace dev{
namespace CTR{
class Dev{
public:
    Handle m_Session;
    Dev(Handle session): 
        m_Session(session) 
    {}

    Result ReadHostIO(bit8 pData[], size_t size, s32 numSectors, SectorSize sectorSize, const bit8 pCommnand[]);
    Result WriteHostIO(const bit8 pData[], size_t size, s32 numSectors, SectorSize sectorSize, const bit8 pCommnand[]);
    Result ReadHostIO2(bit8 pData[], size_t size, s32 sectorOffset, s32 numSectors, SectorSize sectorSize);
    Result WriteHostIO2(const bit8 pData[], size_t size, s32 sectorOffset, s32 numSectors, SectorSize sectorSize);
};
}
}
}
#pragma once

#include <nn/types.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/pxi/CTR/pxi_Names.h>

namespace nn{
namespace dev{
namespace CTR{
    enum SectorSize {
        SECTOR_SIZE_0BYTE   = 0x0,
        SECTOR_SIZE_4BYTE   = 0x1,
        SECTOR_SIZE_16BYTE  = 0x2,
        SECTOR_SIZE_64BYTE  = 0x3,
        SECTOR_SIZE_512BYTE = 0x4,
        SECTOR_SIZE_1KBYTE  = 0x5,
        SECTOR_SIZE_2KBYTE  = 0x6,
        SECTOR_SIZE_4KBYTE  = 0x7,
        SECTOR_SIZE_8KBYTE  = 0x8,
    };

    void Initialize();
    void Finalize();
    Result ReadHostIO(void *pData,s32 numSectors,SectorSize sectorSize,const bit8 pCommand[]);
    Result ReadHostIO2(const void* pData,s32 sectorOffset,s32 numSectors,SectorSize sectorSize);
    Result WriteHostIO(void *pData,s32 numSectors,SectorSize sectorSize,const bit8 pCommand[]);
    Result WriteHostIO2(const void* pData,s32 sectorOffset,s32 numSectors,SectorSize sectorSize);
}
}
}
#pragma once

#include <nn/types.h>

namespace nn{
namespace font{
namespace detail{

typedef u32 SigWord;

 #define NN_FONT_MAKE_SIGWORD(a,b,c,d)        \
    static_cast<SigWord >(                  \
          (static_cast<u8>(a) <<  0)        \
        | (static_cast<u8>(b) <<  8)        \
        | (static_cast<u8>(c) << 16)        \
        | (static_cast<u8>(d) << 24)        \
    )

struct BinaryFileHeader{
    SigWord signature;
    u16 byteOrder;
    u16 headerSize;
    u32 version;
    u32 fileSize;
    u16 dataBlocks;
    u16 reserved;
};

struct BinaryBlockHeader{
    SigWord kind;
    u32 size;
};

bool IsValidBinaryFile(const BinaryFileHeader* pHeader,u32 signature,u32 version,u16 minBlocks = 1);
}
}
}
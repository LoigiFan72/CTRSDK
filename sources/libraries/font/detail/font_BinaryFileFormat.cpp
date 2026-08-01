// Filename: font_BinaryFileFormat.cpp
//
// Project: Horizon

#include <nn/assert.h>
#include <nn/math.h>
#include <nn/font/detail/font_BinaryFileFormat.h>

// literally nw::ut::BinaryFileF shit

namespace nn {
namespace font {
namespace detail {

bool IsValidBinaryFile(const BinaryFileHeader* pHeader,u32 signature, u32 version, u16 minBlocks  /* = 1 */){
    NN_POINTER_ASSERT(pHeader);

    if (pHeader->signature != signature){
        NN_WARNING_(false, "Signature check failed ('%c%c%c%c' must be '%c%c%c%c').",
                   math::GetBits<char>(pHeader->signature, 24, 8),
                   math::GetBits<char>(pHeader->signature, 16, 8),
                   math::GetBits<char>(pHeader->signature,  8, 8),
                   math::GetBits<char>(pHeader->signature,  0, 8),
                   math::GetBits<char>(signature, 24, 8),
                   math::GetBits<char>(signature, 16, 8),
                   math::GetBits<char>(signature,  8, 8),
                   math::GetBits<char>(signature,  0, 8)
        );
        return false;
    }

    if (pHeader->byteOrder != BYTE_ORDER_MARK){
        NN_WARNING_(false, "Unsupported byte order.");
        return false;
    }

    if (NN_FONT_VERSION_MAJOR(version) != NN_FONT_VERSION_MAJOR(pHeader->version) || NN_FONT_VERSION_MINOR(version) < NN_FONT_VERSION_MINOR(pHeader->version)  || NN_FONT_VERSION_BINARYBUGFIX(version) > NN_FONT_VERSION_BINARYBUGFIX(pHeader->version) ){
        NN_WARNING_(false, "Version check faild (bin:'%d.%d.%d.%d', lib:'%d.%d.%d.%d').",
            NN_FONT_VERSION_MAJOR(pHeader->version),
            NN_FONT_VERSION_MINOR(pHeader->version),
            NN_FONT_VERSION_MICRO(pHeader->version),
            NN_FONT_VERSION_BINARYBUGFIX(pHeader->version),
            NN_FONT_VERSION_MAJOR(version),
            NN_FONT_VERSION_MINOR(version),
            NN_FONT_VERSION_MICRO(version),
            NN_FONT_VERSION_BINARYBUGFIX(version)
        );
        return false;
    }

    if(pHeader->fileSize < sizeof(BinaryFileHeader) + sizeof(BinaryBlockHeader) * minBlocks){
        NN_WARNING_(false, "Too small file size(=%d).", pHeader->fileSize);
        return false;
    }

    if(pHeader->dataBlocks < minBlocks){
        NN_WARNING_(false, "Too small number of data blocks(=%d).", pHeader->dataBlocks);
        return false;
    }

    return true;
}

}
}
}
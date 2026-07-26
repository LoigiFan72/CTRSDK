#pragma once

#include <nn/gx/CTR/gx_PicaCommon.h>

enum PicaDataVertexAttrType{
    PICA_DATA_SIZE_1_BYTE          = 0x0,
    PICA_DATA_SIZE_1_UNSIGNED_BYTE = 0x1,
    PICA_DATA_SIZE_1_SHORT         = 0x2,
    PICA_DATA_SIZE_1_FLOAT         = 0x3,
    PICA_DATA_SIZE_2_BYTE          = 0x4,
    PICA_DATA_SIZE_2_UNSIGNED_BYTE = 0x5,
    PICA_DATA_SIZE_2_SHORT         = 0x6,
    PICA_DATA_SIZE_2_FLOAT         = 0x7,
    PICA_DATA_SIZE_3_BYTE          = 0x8,
    PICA_DATA_SIZE_3_UNSIGNED_BYTE = 0x9,
    PICA_DATA_SIZE_3_SHORT         = 0xa,
    PICA_DATA_SIZE_3_FLOAT         = 0xb,
    PICA_DATA_SIZE_4_BYTE          = 0xc,
    PICA_DATA_SIZE_4_UNSIGNED_BYTE = 0xd,
    PICA_DATA_SIZE_4_SHORT         = 0xe,
    PICA_DATA_SIZE_4_FLOAT         = 0xf
};

enum PicaDataDrawMode{
    PICA_DATA_DRAW_TRIANGLE_STRIP      = 0x1,
    PICA_DATA_DRAW_TRIANGLE_FAN        = 0x2,
    PICA_DATA_DRAW_TRIANGLES           = 0x3,
    PICA_DATA_DRAW_GEOMETRY_PRIMITIVE  = 0x3
};
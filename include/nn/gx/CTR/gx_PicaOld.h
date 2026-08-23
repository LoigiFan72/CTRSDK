#pragma once

enum{
    PICA_DATA_TEXTURE_FORMAT_HILO8_DMP_UNSIGNED_BYTE_DMP = 0x6
};

enum{    
    PICA_REG_VTX_ATTR_ARRAYS_BASE_ADDR = 0x200,
    PICA_REG_VTX_ATTR_ARRAYS0          = 0x201,
    PICA_REG_VTX_ATTR_ARRAYS1          = 0x202,
    
    PICA_REG_VTX_FUNC                  = 0x231,

    PICA_REG_GS_PROG_RENEWAL_END       = 0x28f,
    PICA_REG_GS_FLOAT_DATA1            = 0x291,
    PICA_REG_GS_FLOAT_DATA2            = 0x292,
    PICA_REG_GS_FLOAT_DATA3            = 0x293,
    PICA_REG_GS_FLOAT_DATA4            = 0x294,
    PICA_REG_GS_FLOAT_DATA5            = 0x295,
    PICA_REG_GS_FLOAT_DATA6            = 0x296,
    PICA_REG_GS_FLOAT_DATA7            = 0x297,
    PICA_REG_GS_FLOAT_DATA8            = 0x298,

    PICA_REG_VS_PROG_RENEWAL_END       = 0x2bf,
    PICA_REG_VS_FLOAT_DATA1            = 0x2c1,
    PICA_REG_VS_FLOAT_DATA2            = 0x2c2,
    PICA_REG_VS_FLOAT_DATA3            = 0x2c3,
    PICA_REG_VS_FLOAT_DATA4            = 0x2c4,
    PICA_REG_VS_FLOAT_DATA5            = 0x2c5,
    PICA_REG_VS_FLOAT_DATA6            = 0x2c6,
    PICA_REG_VS_FLOAT_DATA7            = 0x2c7,
    PICA_REG_VS_FLOAT_DATA8            = 0x2c8
};

#define PICA_CMD_DATA_LOAD_ARRAY0_ELEMENT(attr0, attr1, attr2, attr3, attr4, attr5, attr6, attr7) \
   ( (attr0)       | (attr1) << 4  | (attr2) << 8  | (attr3) << 12 | \
     (attr4) << 16 | (attr5) << 20 | (attr6) << 24 | (attr7) << 28)

#define PICA_CMD_DATA_VTX_ATTR_ARRAYS_BASE_ADDR(addr) ( ((u32)(addr) >> 4) << 1 )

#define PICA_CMD_DATA_VERTEX_ATTR_ARRAYS(attr0, attr1, attr2, attr3, attr4, attr5, attr6, attr7) \
   ((attr0)       | (attr1) <<  4 | (attr2) <<  8 | (attr3) << 12 | \
    (attr4) << 16 | (attr5) << 20 | (attr6) << 24 | (attr7) << 28)
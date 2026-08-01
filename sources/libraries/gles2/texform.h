#pragma once

enum TEXTURE_FORMAT{
	TF_R8G8B8A8 = 0,
	TF_R8G8B8   = 1,
	TF_R5G5B5A1 = 2,
	TF_R5G6B5   = 3,
	TF_R4G4B4A4 = 4,
	TF_L8A8     = 5,
	TF_R8G8     = 6,
	TF_L8       = 7,
	TF_A8       = 8,
	TF_L4A4     = 9,
	TF_L4       = 10,
	TF_A4       = 11,
	TF_ETC      = 12,
	TF_ETCA4    = 13,
	
	TF_MAX_BIT = (1u << 30)
};

enum C_FORMAT{
	CF_R8G8B8A8 = 0,
	CF_R8G8B8   = 1,
	CF_R5G5B5A1 = 2,
	CF_R5G6B5   = 3,
	CF_R4G4B4A4 = 4,
	
	CF_MAX_BIT = (1u << 30)
};

enum Z_FORMAT{
	ZF_Z16      = 0,
	ZF_Z16S8    = 1,
	ZF_Z24      = 2,
	ZF_Z24S8    = 3,
	
	ZF_MAX_BIT = (1u << 30)
};
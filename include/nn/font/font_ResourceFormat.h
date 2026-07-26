#pragma once

#include <nn/types.h>
#include <nn/font/detail/font_BinaryFileFormat.h>

using namespace nn::font::detail;

namespace nn {
namespace font {

// "CFNT"

const SigWord BINFILE_SIG_FONT = NN_FONT_MAKE_SIGWORD('C','F','N','T');

// "CFNU"
const SigWord BINFILE_SIG_FONT_RESOLEVED = NN_FONT_MAKE_SIGWORD('C','F','N','U');

// "CFNA"
const SigWord BINFILE_SIG_FONTA = NN_FONT_MAKE_SIGWORD('C','F','N','A');

const SigWord BINBLOCK_SIG_FINF = NN_FONT_MAKE_SIGWORD('F','I','N','F');
const SigWord BINBLOCK_SIG_CGLP = NN_FONT_MAKE_SIGWORD('C','G','L','P');
const SigWord BINBLOCK_SIG_TGLP = NN_FONT_MAKE_SIGWORD('T','G','L','P');
const SigWord BINBLOCK_SIG_CWDH = NN_FONT_MAKE_SIGWORD('C','W','D','H');
const SigWord BINBLOCK_SIG_CMAP = NN_FONT_MAKE_SIGWORD('C','M','A','P');

const SigWord BINBLOCK_SIG_GLGR = NN_FONT_MAKE_SIGWORD('G','L','G','R');
const SigWord BINBLOCK_SIG_HTGL = NN_FONT_MAKE_SIGWORD('H','T','G','L');

const u32 FONT_FILE_VERSION  = NN_FONT_MAKE_SIGWORD(3, 0, 0, 0);

const u16 INVALID_CHAR_CODE   = 0xFFFF;

enum FontType{
    FONT_TYPE_GLYPH,
    FONT_TYPE_TEXTURE,
    NUM_OF_FONT_TYPE
};

enum CharacterCode{
    CHARACTER_CODE_UNICODE = 1,
    CHARACTER_CODE_SJIS,
    CHARACTER_CODE_CP1252,
    CHARACTER_CODE_MAX
};

enum FontMapMethod{
    FONT_MAPMETHOD_DIRECT,
    FONT_MAPMETHOD_TABLE,
    FONT_MAPMETHOD_SCAN,
    NUM_OF_FONT_MAPMETHOD
};

enum FontSheetFormat{
    FONT_SHEET_FORMAT_RGBA8,
    FONT_SHEET_FORMAT_RGB8,
    FONT_SHEET_FORMAT_RGB5A1,
    FONT_SHEET_FORMAT_RGB565,
    FONT_SHEET_FORMAT_RGBA4,
    FONT_SHEET_FORMAT_LA8,

    FONT_SHEET_FORMAT_A8 = 8,
    FONT_SHEET_FORMAT_LA4,

    FONT_SHEET_FORMAT_A4 = 11,

    FONT_SHEET_FORMAT_MASK = 0x7FFF,
    FONT_SHEET_FORMAT_COMPRESSED_FLAG = 0x8000
};

struct CharWidths{
    s8 left;
    u8 glyphWidth;
    s8 charWidth;
};

struct CMapScanEntry{
    u16 ccode;
    u16 index;
};

struct CMapInfoScan{
    u16 num;
    CMapScanEntry* GetEntries() const{
        return reinterpret_cast<CMapScanEntry*>( reinterpret_cast<uptr>(this) + sizeof(*this) );
    }
};

struct FontGlyphGroups{
    u32 sheetSize;
    u16 glyphsPerSheet;
    u16 numSet;
    u16 numSheet;
    u16 numCWDH;
    u16 numCMAP;

    u16 nameOffsets[1];
};

struct FontWidth{
    ushort indexBegin;
    ushort indexEnd;
    FontWidth* pNext;
    CharWidths* GetWidthTable() const;
};

struct FontCodeMap{
    ushort ccodeBegin;
    ushort ccodeEnd;
    ushort mappingMethod;
    ushort reserved;
    FontCodeMap* pNext;
    ushort* GetMapInfo() const;
};

struct FontTextureGlyph{
    u8 cellWidth;
    u8 cellHeight;
    s8 baselinePos;
    u8 maxCharWidth;
    u32 sheetSize;
    u16 sheetNum;
    u16 sheetFormat;
    u16 sheetRow;
    u16 sheetLine;
    u16 sheetWidth;
    u16 sheetHeight;
    u8* sheetImage;
};

struct FontInformation{
    u8 fontType;
    s8 linefeed;
    ushort alterCharIndex;
    CharWidths defaultWidth;
    u8 characterCode;
    FontTextureGlyph* pGlyph;
    FontWidth* pWidth;
    FontCodeMap* pMap;
    u8 height;
    u8 width;
    u8 ascent;
    u8 padding_[1];
};


}
}
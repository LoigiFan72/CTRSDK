// Filename: font_ResFont.cpp
//
// Project: Horizon

#include <nn/Assert.h>
#include <nn/font/font_ResFont.h>

namespace nn {
namespace font {

ResFont::ResFont(){ }

ResFont::~ResFont(){
    if (!this->IsManaging(NULL)){
    	this->RemoveResource();
	}
}

u32 ResFont::GetDrawBufferSize(const void* bfnt){
    const FontTextureGlyph* pGlyph = NULL;
    const detail::BinaryFileHeader* fileHeader = reinterpret_cast<const detail::BinaryFileHeader*>(bfnt);

    if (fileHeader->signature == BINFILE_SIG_FONT_RESOLEVED){ }
    else{
        if (!IsValidBinaryFile(fileHeader, 0x544e4643, 0x3000000, 2)){
            return 0;
        }
    }
    
    const detail::BinaryBlockHeader* blockHeader = reinterpret_cast<const detail::BinaryBlockHeader*>(reinterpret_cast<const u8*>(fileHeader) + fileHeader->headerSize);

    int nBlocks = 0;
    while (nBlocks < fileHeader->dataBlocks){
        NN_POINTER_ASSERT(blockHeader);
        if (blockHeader->kind == BINBLOCK_SIG_TGLP){
            pGlyph = reinterpret_cast<const FontTextureGlyph*>(reinterpret_cast<const u8*>(blockHeader) + sizeof(*blockHeader));
            break;
        }

        blockHeader = reinterpret_cast<const detail::BinaryBlockHeader*>(reinterpret_cast<const u8*>(blockHeader) + blockHeader->size);
        nBlocks++;
    }

    if (pGlyph == NULL){
        return 0;
    }

    return sizeof(internal::TextureObject) * pGlyph->sheetNum;
}

void* ResFont::SetDrawBuffer(void* buffer){
    void *const prevBuffer = this->GetTextureObjectsBufferPtr();
    if (prevBuffer == buffer){
        return buffer;
    }

    if (NULL != prevBuffer){
        this->DeleteTextureNames();
    }

    this->SetTextureObjectsBufferPtr(buffer);

    if (NULL != buffer){
        this->GenTextureNames();
    }

    return prevBuffer;
}

}
}
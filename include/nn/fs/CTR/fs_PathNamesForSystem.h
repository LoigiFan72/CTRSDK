#pragma once

#include <nn/types.h>
#include <nn/util/util_Int64.h>
#include <nn/util/util_SizedEnum.h>

namespace nn{
namespace fs{
namespace CTR{

struct ExeFsSectionNames{};

struct ContentPath : public ExeFsSectionNames{
    enum Tag{
        TAG_ROMFS,
        TAG_EXEFS,
        TAG_SYSTEM_MENU_DATA,
        TAG_PROGRAM_INFO
    };

    nn::util::SizedEnum4<Tag> tag;
    nn::util::Int64<bit64> id;
    
    static ContentPath MakeExeFsPath(nn::util::Int64<bit64> id){
        ContentPath ret;
        ret.tag = TAG_EXEFS;
        ret.id = id;
        return ret;
    }
    
    static ContentPath MakeSystemMenuDataPath(nn::util::Int64<bit64> id){
        ContentPath ret;
        ret.tag = TAG_SYSTEM_MENU_DATA;
        ret.id = id;
        return ret;
    }
    
    static ContentPath MakeRomfsPath(){
        ContentPath ret;
        ret.tag = TAG_ROMFS;
        ret.id = 0;
        return ret;
    }
    
    ContentPath& SetAllIdBits(){
        this->id = 0xFFFFFFFFFFFFFFFFLLU;
        return *this;
    }
    
    ContentPath& ClearAllIdBits(){
        this->id = 0;
        return *this;
    }
    
    ContentPath& SetIdBit(int index){
        if (0 <= index && index < 64){
            this->id |= (1LLU << index);
        }
        return *this;
    }
    
    static ContentPath MakeProgramInfoPath(bit32 id){
        ContentPath ret;
        ret.tag = TAG_PROGRAM_INFO;
        ret.id = id;
        return ret;
    }
    
};

struct ProgramDataPath{
    enum Tag{
        TAG_ROMFS_DEFAULT,
        TAG_EXEFS,
        TAG_SYSTEM_MENU_DATA,
        TAG_SAVE_DATA,
        TAG_CONTENT,
        TAG_ROMFS_EXTRA
    };
    
    nn::util::SizedEnum4<Tag> tag;
    nn::util::Int64<bit64> id;

    static ProgramDataPath MakeRomFsDefaultPath(){
        ProgramDataPath ret;
        ret.tag = TAG_ROMFS_DEFAULT;
        ret.id = 0;
        return ret;
    }
};

struct TitleDataPath{
    enum Tag{
        TAG_CONTENT_FILE,
        TAG_SAVE_DATA_FILE,
        TAG_LEGACY_CONTENT_FILE,
        TAG_LEGACY_SUBBANNER_FILE,
        TAG_ENCRYPTED_RAW_SAVE_DATA_FILE,
        TAG_ENCRYPTED_RAW_SAVE_DATA_FILE_CORE
    };
        
    nn::util::SizedEnum4<Tag> tag;
    bit32 data[4];

    void SetContentIdx(nn::fs::ContentIdx contentIdx) { this->data[0] = contentIdx; }
    nn::fs::ContentIdx GetContentIdx() const { return data[0]; }
        
    void SetContentPath(const ContentPath& contentPath){
        reinterpret_cast<ContentPath&>(this->data[1]) = contentPath;
    }
        
    const ContentPath& GetContentPath() const{
        return reinterpret_cast<const ContentPath&>(this->data[1]);
    }
        
    bit32 GetToken() const{
        return data[0];
    }
        
    static TitleDataPath MakeSaveDataFilePath(){
        TitleDataPath ret;
        ret.tag = TAG_SAVE_DATA_FILE;
        return ret;
    }
        
    static TitleDataPath MakeContentDataPath(nn::fs::ContentIdx contentIdx, const ContentPath& contentPath){
        TitleDataPath ret;
        ret.tag = TAG_CONTENT_FILE;
        ret.SetContentIdx(contentIdx);
        ret.SetContentPath(contentPath);
        return ret;
    }
};

}
}
}
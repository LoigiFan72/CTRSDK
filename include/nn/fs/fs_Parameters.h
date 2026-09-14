#pragma once

#include <nn/types.h>
#include <nn/util/util_Int64.h>
#include <nn/util/util_SizedEnum.h>

namespace nn{
namespace fs{
namespace detail { struct ArchiveHandleTag {}; }
    typedef nn::util::Int64<bit64, detail::ArchiveHandleTag> ArchiveHandle;
    typedef bit64 ExtSaveDataId;
    typedef bit32 ContentIdx;
    typedef s64 TitleId;

    static const size_t MAX_ARCHIVE_NAME_LENGTH = 7;
    static const size_t MAX_SUB_PATH_LENGTH = 253;
    static const size_t MAX_FILE_PATH_LENGTH = MAX_ARCHIVE_NAME_LENGTH + 1 + MAX_SUB_PATH_LENGTH;
    
    struct Attributes
    {
        bool isDirectory;
        bool isHidden;
        bool isArchive;
        bool isReadOnly;
    };

    enum StorageAttribute
    {
        NORMAL = 0,
        FIXED,
        NEW,
    };

    enum MediaType
    {
        MEDIA_TYPE_NAND = 0,
        MEDIA_TYPE_SDMC,
        MEDIA_TYPE_CTRCARD
    };

    struct ShortName
    {
        char body[10];
        char ext[4];
        bool valid;
        bit8 pad;
    };

    struct DirectoryEntry
    {
        wchar_t entryName[MAX_FILE_PATH_LENGTH + 1];
        ShortName shortName;
        nn::fs::Attributes attributes;
        s64 entrySize;
    };

    enum OpenMode
    {
        OPEN_MODE_READ    = (1u << 0),
        OPEN_MODE_WRITE   = (1u << 1),
        OPEN_MODE_CREATE  = (1u << 2)
    };

    struct ProgramDataPath
    {
        enum Tag
        {
            TAG_ROMFS_DEFAULT,
            TAG_EXEFS,
            TAG_SYSTEM_MENU_DATA,
            TAG_SAVE_DATA,
            TAG_CONTENT,
            TAG_ROMFS_EXTRA
        };

        util::SizedEnum4<Tag> tag;
        util::Int64<bit64> id;
    };

    struct TitleDataSpecifier
    {
        TitleId id;
        nn::util::SizedEnum1<MediaType> media;
        s8 rev1[3];
        s32 rev2;
        
        static TitleDataSpecifier Make(MediaType media, TitleId id)
        {
            TitleDataSpecifier ret;
            ret.id = id;
            ret.media = media;
            return ret;
        }

        void CopyTo(TitleDataSpecifier* p) const
        {
            p->id =    this->id;
            p->media = this->media;
        }
    };

    struct ExtSaveDataSpecifier
    {
        nn::util::SizedEnum1<MediaType> mediaType;
        nn::util::SizedEnum1<StorageAttribute> storageAttribute;
        bit8 reserved[2];
        nn::util::Int64<ExtSaveDataId> extSaveDataId;

        static ExtSaveDataSpecifier Make(MediaType mediaType, ExtSaveDataId extSaveDataId)
        {
            return Make(mediaType, NORMAL, extSaveDataId);
        }
        
        static ExtSaveDataSpecifier Make(MediaType mediaType, StorageAttribute storageAttribute, ExtSaveDataId extSaveDataId)
        {
            ExtSaveDataSpecifier ret = {};
            ret.mediaType = mediaType;
            ret.storageAttribute = storageAttribute;
            ret.extSaveDataId = extSaveDataId;
            return ret;
        }
    };

    typedef ExtSaveDataSpecifier ExtSaveDataArchivePath;

    struct TitleDataSpecificer
    {
        TitleId id;
        MediaType media;
        s8 unkpad_1[3];
        int unkflag1;
    };

    struct DataContentArchivePath
    {
        TitleId titleId;
        nn::util::SizedEnum4<MediaType> mediaType;
        ContentIdx contentIdx;
    };

    struct WriteOption
    {
        bool flush;
        bool updateTimeStampOld;
        bool updateTimeStamp;

        struct
        {
            bit8 destroySignature:1;
            bit8 reserved3:7;
        } system;
        bit8 reserved3;

        WriteOption(bool flush, bool updateTimeStamp):
            flush(flush), 
            updateTimeStampOld(false), 
            updateTimeStamp(updateTimeStamp)
        {
        }
    };

    struct Transaction
    {
        bit32 dummy;
    };

    enum PositionBase
    {
        BASE_BEGIN = 0,
        BASE_CURRENT,
        BASE_END,
    };

    enum SystemMediaType
    {
        CTR_NAND = 0,
        TWL_NAND,
        SDMC,
        TWL_PHOTO,
    };
    
}
}
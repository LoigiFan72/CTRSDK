#pragma once

#include <nn/fs/CTR/MPCore/fs_UserArchive.h>

namespace nn{
namespace fs{
namespace CTR{
namespace MPCore{
namespace detail{

class ArchiveName{
public:
    ArchiveName(const wchar_t* path){
        s64 id = 0;

        for (s32 i = 0; i < 8; ++i){
            if (path[i] == L':'){
                this->id = id;
                return;
            }
            id = (id << 8) | (char)path[i];
        }
        this->id = 0;
    }

    ArchiveName(const char* path){
        s64 id = 0;

        for (s32 i = 0; i < 8; ++i){
            if (path[i] == ':'){
                this->id = id;
                return;
            }
            id = (id << 8) | (char)path[i];
        }
        this->id = 0;
    }

    bit64 id;
};

struct ArchiveTableEntry{
    bit64 id;
    IArchive* archive;
    bool isAddOnContent;
    bool isAlias;
    s16 r;

    ArchiveTableEntry() : id(0), archive(0) {}
};

const size_t NUM_ARCHIVE = 32;

ArchiveTableEntry gArchiveTable[NUM_ARCHIVE];

template <typename TString_>
ArchiveTableEntry* FindArchive(const TString_* path, int){
    ArchiveName name(path);

    if (name.id != 0){
        for (s32 i = 0; i < NUM_ARCHIVE; ++i){
            if (name.id == gArchiveTable[i].id){
                return gArchiveTable + i;
            }
        }
    }

    return 0;
}

template <typename TString_>
IArchive* FindArchive(const TString_* path){
    ArchiveTableEntry* table = FindArchive(path, 0);
    return table ? table->archive : 0;
}

inline ArchiveName MakeArchiveName(const char* path){
    char buf[8];
    for (int i = 0; i < 8; ++i){
        if (path[i] == 0 || path[i] == ':'){
            buf[i] = ':';
            return ArchiveName(buf);
        }
        buf[i] = path[i];
    }
    NN_TPANIC_("invalid archive name");
    return "";
}

Result RegisterArchive(const char* path, IArchive* archive, bool isAddOnContent, bool isAlias){
    ArchiveName name = MakeArchiveName(path);
    if (!name.id){
        return ResultInvalidArgument();
    }

    for (s32 i = 0; i < NUM_ARCHIVE; ++i){
        if (gArchiveTable[i].archive == 0){
            gArchiveTable[i].id = name.id;
            gArchiveTable[i].archive = archive;
            gArchiveTable[i].isAddOnContent = isAddOnContent;
            gArchiveTable[i].isAlias = isAlias;
            return ResultSuccess();
        }

        if (gArchiveTable[i].id == name.id){
            return ResultAlreadyExists();
        }
    }
    return ResultOutOfMemory();
}

inline Result RegisterArchive(const char* path, IArchive* archive){ return RegisterArchive(path, archive, false, false); }
inline Result RegisterArchive(const char* path, IArchive* archive, bool isAddOnContent){ return RegisterArchive(path, archive, isAddOnContent, false);  }

IArchive* UnregisterArchive(bool* pIsAlias, const ArchiveName& name){
    for (s32 i = 0; i < NUM_ARCHIVE; ++i){
        if (gArchiveTable[i].id == name.id){
            IArchive* ret = gArchiveTable[i].archive;
            *pIsAlias = gArchiveTable[i].isAlias;
            gArchiveTable[i].id = 0;
            gArchiveTable[i].archive = 0;
            return ret;
        }
    }
    return 0;
}


}
}
}
}
}
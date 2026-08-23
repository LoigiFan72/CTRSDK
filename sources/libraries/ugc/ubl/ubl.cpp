

#include <nn/ubl/ubl_Api.h>
#include <nn/cfg.h>
#include <nn/init.h>
#include <nn/fs.h>
#include <nn/fs/CTR/MPCore/fs_FileSystemBase.h>

namespace nn{
namespace ubl{
namespace{

const nn::fnd::TimeSpan RETRY_INTERVAL = nn::fnd::TimeSpan::FromMilliSeconds(10);
const s32 RETRY_MAX = 100;

typedef struct{
    union{
        u8  cid[8];
        u64 uid;
    };
    union{
        u32 dateTime;
        struct{
            u32 second:3;
            u32 minute:6;
            u32 hour:5;
            u32 day:5;
            u32 month:4;
            u32 year:9;
        };
    };
} LocalBlackList;

static LocalBlackList sLocalBlackList[MAX_LOCAL_BLACK_LIST];

#define UBL_VERSION     0x10110811

#define NG_TIME             7

#ifdef NN_UBL_ENABLE_GLOBAL_BLACKLIST

typedef struct{
    union{
        u8  cid[8];
        u64 uid;
    };
    u32 titleId;
} GlobalUserBlackList;

typedef struct{
    union{
        u8  cid[8];
        u64 did;
    };
    u32 titleId;
} GlobalDataBlackList;

#define MAX_GLOBAL_BLACK_USERLIST   500
#define MAX_GLOBAL_BLACK_DATALIST   7600

#define DATA_ID_LENGTH  8

#define RESERVED_LENGTH 128

static struct{
    u32 version;
    u32 userSize;
    u32 dataSize;
    u8  reserved[RESERVED_LENGTH - 12];
    GlobalUserBlackList *userBlackList;
    GlobalDataBlackList *dataBlackList;
} sGlobalBlackList;

#endif

const char* sArchiveName           = "ubl_:";
const bit32 sSharedExtSaveDataId   = 0xF000000B;

typedef struct{
    wchar_t *fileName;
    s32 fileSize;
} UBL_Setting;

static const UBL_Setting sSettingTable[] ={
    { L"ubl_:/ubll.lst", 12000 }
#ifdef NN_UBL_ENABLE_GLOBAL_BLACKLIST
    ,{ L"ubl_:/ublg.lst", 97328}
#endif
};
static bool sIsInitialized = false;

static nn::Result WriteLocalBlackList(void);

}

Result Initialize(void){
// TODO
}

void Finalize(void){
    if (sIsInitialized){
#ifdef NN_UBL_ENABLE_GLOBAL_BLACKLIST
        nn::fnd::IAllocator* pAllocator = nn::init::GetAllocator();

        if (sGlobalBlackList.userBlackList != NULL){
            pAllocator->Free(sGlobalBlackList.userBlackList);
        }
        if (s_GlobalBlackList.dataBlackList != NULL){
            pAllocator->Free(sGlobalBlackList.dataBlackList);
        }
#endif // #ifdef NN_UBL_ENABLE_GLOBAL_BLACKLIST
        sIsInitialized = false;
    }
}




}
}
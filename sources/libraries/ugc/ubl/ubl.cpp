

#include <nn/ubl/ubl_Api.h>
#include <nn/cfg.h>
#include <nn/init.h>
#include <nn/fs.h>
#include <nn/fs/fs_FileSystemBase.h>
#include <nn/fs/CTR/MPCore/fs_FileSystemBase.h>

namespace nn{
namespace ubl{

const u32 MAX_LOCAL_BLACK_LIST  = 1000;

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

static LocalBlackList sLocalBlackList[2]; // 2 Lists

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

const char* s_ArchiveName           = "ubl_:";
const bit32 s_SharedExtSaveDataId   = 0xF000000B;

typedef struct{
    wchar_t *fileName;
    s32 fileSize;
} UBL_Setting;

static const UBL_Setting s_SettingTable[] ={
    { L"ubl_:/ubll.lst", 12000 }
#ifdef NN_UBL_ENABLE_GLOBAL_BLACKLIST
    ,{ L"ubl_:/ublg.lst", 97328}
#endif
};
static bool isInitialized = false;

static nn::Result WriteLocalBlackList(void);

}

Result Initialize(){
    if(isInitialized){
        return nn::Result(nn::Result::LEVEL_PERMANENT, nn::Result::SUMMARY_NOTHING_HAPPENED, nn::Result::MODULE_NN_NGC, nn::Result::DESCRIPTION_ALREADY_INITIALIZED);
    }

    Result result;

    nn::fs::Initialize();

    s64 fileSize;
    nn::fs::FileInputStream fr;
    nn::Result result;

    bool isError = true;
    s32 retryCount = 0;

    result = nn::fs::MountSharedExtSaveData(s_ArchiveName, s_SharedExtSaveDataId);

    if (result.IsFailure()){
        return nn::Result(nn::Result::LEVEL_FATAL, nn::Result::SUMMARY_INTERNAL, nn::Result::MODULE_NN_NGC, nn::Result::DESCRIPTION_INVALID_RESULT_VALUE);
    }

    do{
        result = fr.TryInitialize(s_SettingTable[0].fileName);

        if (result.IsFailure()){

            if (result <= nn::fs::ResultOperationDenied()){
                if (++retryCount >= RETRY_MAX){
                    break;
                }
                continue;
            }

            break;
        }

        result = fr.TryGetSize(&fileSize);

        if (result.IsSuccess() && fileSize == sizeof(LocalBlackList) * MAX_LOCAL_BLACK_LIST){
            s32 readSize;
            result = fr.TryRead(&readSize, sLocalBlackList, static_cast<size_t>(fileSize));

            if (result.IsSuccess()){
                isError = false;
            }
        }

        fr.Finalize();
        break;
    } while (true);

    if (isError){
        for (int i = 0; i < MAX_LOCAL_BLACK_LIST; i++){
            sLocalBlackList[i].uid = 0;
            sLocalBlackList[i].dateTime = NG_TIME;
        }

        nn::fs::CreateFile(s_SettingTable[0].fileName, s_SettingTable[0].fileSize);
        result = WriteLocalBlackList();
        if(result.IsFailure()){
            return nn::Result(nn::Result::LEVEL_FATAL, nn::Result::SUMMARY_INTERNAL, nn::Result::MODULE_NN_NGC, nn::Result::DESCRIPTION_NOT_AUTHORIZED);
        }
    }
#ifdef ENABLE_GLOBAL_BLACK_LIST
    if (!enableGlobalBlackList){
        sGlobalBlackList.version = UBL_VERSION;
        sGlobalBlackList.userSize =
        sGlobalBlackList.dataSize = 0;
        std::memset(sGlobalBlackList.reserved, 0, RESERVED_LENGTH - 12);

        sGlobalBlackList.userBlackList = NULL;
        sGlobalBlackList.dataBlackList = NULL;
    }
    else{
        s32 memSize;

        nn::fnd::IAllocator* pAllocator = nn::init::GetAllocator();

        retryCount = 0;
        isError = true;

        {
            sGlobalBlackList.userBlackList = NULL;
            sGlobalBlackList.dataBlackList = NULL;
        }

        do{
            result = fr.TryInitialize(s_SettingTable[1].fileName);

            if (result.IsFailure()){
                if (result <= nn::fs::ResultOperationDenied()){
                    if (++retryCount >= RETRY_MAX){
                        break;
                    }
                    continue;
                }

                break;
            }

            result = fr.TryGetSize(&fileSize);

            if (result.IsSuccess() && fileSize >= RESERVED_LENGTH){
                s32 readSize;

                result = fr.TryRead(&readSize, &sGlobalBlackList, RESERVED_LENGTH);

                if (result.IsSuccess()){
                    isError = false;

                    if (sGlobalBlackList.userSize > 0){

                        if (sGlobalBlackList.userSize > MAX_GLOBAL_BLACK_USERLIST){
                            sGlobalBlackList.userSize = 0;
                            sGlobalBlackList.dataSize = 0;

                            fr.Finalize();
                            break;
                        }
                        memSize = sizeof(GlobalUserBlackList) * sGlobalBlackList.userSize;
                        sGlobalBlackList.userBlackList = static_cast<GlobalUserBlackList*>(pAllocator->Allocate(memSize, 4));

                        if (sGlobalBlackList.userBlackList != NULL){
                            result = fr.TryRead(&readSize, sGlobalBlackList.userBlackList, memSize);

                            if (result.IsFailure()){
                                pAllocator->Free(sGlobalBlackList.userBlackList);
                                sGlobalBlackList.userBlackList = NULL;

                                sGlobalBlackList.userSize = 0;
                                sGlobalBlackList.dataSize = 0;

                                fr.Finalize();
                                break;
                            }
                        }
                        else{
                            sGlobalBlackList.userSize = 0;

                            result = fr.TrySeek(memSize, nn::fs::POSITION_BASE_CURRENT);
                            if (result.IsFailure()){
                                sGlobalBlackList.dataSize = 0;

                                fr.Finalize();
                                break;
                            }
                        }
                    }
                    else{
                        sGlobalBlackList.userBlackList = NULL;
                        sGlobalBlackList.userSize = 0;
                    }

                    if (sGlobalBlackList.dataSize > 0){

                        if (sGlobalBlackList.dataSize > MAX_GLOBAL_BLACK_DATALIST){

                            sGlobalBlackList.dataSize = 0;

                            fr.Finalize();
                            break;
                        }
                        memSize = sizeof(GlobalDataBlackList) * sGlobalBlackList.dataSize;
                        sGlobalBlackList.dataBlackList = static_cast<GlobalDataBlackList*>(pAllocator->Allocate(memSize, 4));

                        if (sGlobalBlackList.dataBlackList != NULL){
                            result = fr.TryRead(&readSize, sGlobalBlackList.dataBlackList, memSize);

                            if (result.IsFailure()){
                                pAllocator->Free(sGlobalBlackList.dataBlackList);
                                sGlobalBlackList.dataBlackList = NULL;

                                sGlobalBlackList.dataSize = 0;

                                fr.Finalize();
                                break;
                            }
                        }
                        else{
                            sGlobalBlackList.dataSize = 0;

                            result = fr.TrySeek(memSize, nn::fs::POSITION_BASE_CURRENT);
                            if (result.IsFailure()){
                                fr.Finalize();
                                break;
                            }
                        }
                    }
                    else{
                        sGlobalBlackList.dataBlackList = NULL;
                        sGlobalBlackList.dataSize = 0;
                    }
                }
            }
            fr.Finalize();
        } while (true);

        if (isError){
            sGlobalBlackList.version = UBL_VERSION;
            sGlobalBlackList.userSize =
            sGlobalBlackList.dataSize = 0;
            std::memset(sGlobalBlackList.reserved, 0, RESERVED_LENGTH - 12);
        }
    }
#endif//#ifdef NN_UBL_ENABLE_GLOBAL_BLACK_LIST
    return nn::ResultSuccess();
}

void Finalize(){
    if (isInitialized){
        nn::fs::Unmount(s_ArchiveName);
#ifdef NN_UBL_ENABLE_GLOBAL_BLACKLIST
        nn::fs::Unmount(s_ArchiveName);
        nn::fnd::IAllocator* pAllocator = nn::init::GetAllocator();

        if (sGlobalBlackList.userBlackList != NULL){
            pAllocator->Free(sGlobalBlackList.userBlackList);
        }
        if (sGlobalBlackList.dataBlackList != NULL){
            pAllocator->Free(sGlobalBlackList.dataBlackList);
        }
#endif // #ifdef NN_UBL_ENABLE_GLOBAL_BLACKLIST
        isInitialized = false;
    }
}

bool IsExist(u64 authorId, u32 titleId, u64 dataId){
    u32 i;
    bool found = false;

    if (!isInitialized){
        return false;
    }

    for (i = 0; i < MAX_LOCAL_BLACK_LIST; i++){
        if (sLocalBlackList[i].uid == authorId && sLocalBlackList[i].dateTime != NG_TIME){
            found = true;
            break;
        }
    }
#ifdef NN_UBL_ENABLE_GLOBAL_BLACK_LIST
    u32 size;

    if (found != true){
        size = sGlobalBlackList.userSize;
        for (i = 0; i < size; i++){
            if (sGlobalBlackList.userBlackList[i].uid == authorId && sGlobalBlackList.userBlackList[i].titleId == titleId){
                found = true;
                break;
            }
        }
        if (found != true){
            size = sGlobalBlackList.dataSize;
            for (i = 0; i < size; i++){
                if (sGlobalBlackList.dataBlackList[i].titleId == titleId && sGlobalBlackList.dataBlackList[i].did == dataId){
                    found = true;
                    break;
                }
            }
        }
    }
#endif//#ifdef NN_UBL_ENABLE_GLOBAL_BLACK_LIST
    return found;
}

u64 GetUserId(){
    return nn::cfg::CTR::GetTransferableId(0);
}

namespace{

static Result WriteLocalBlackList(){
    FileOutputStream fw;
    Result result = fw.TryInitialize(s_SettingTable[1].fileName, false);
    do{
        if(result.IsSuccess()){
            s32 writeSize;
            fw.TryWrite(&writeSize, sLocalBlackList, sizeof(LocalBlackList) * MAX_LOCAL_BLACK_LIST, true);
            fw.Finalize();
        }
        break;
    }while(true);

    return result;
}

}


}
}
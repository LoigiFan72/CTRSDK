// Filename: [MPCORE] fs_Api.cpp
//
// Project: Horizon

#include <nn/fs/fs_Api.h>
#include <nn/fs/fs_Parameters.h>
#include <nn/fs/fs_FileSystemBase.h>
#include <nn/err/CTR/err_Api.h>
#include <nn/fnd/fnd_TimeSpan.h>
#include <nn/cfg/CTR/cfg_Api.h>
#include <nn/srv.h>

#include <string.h>

namespace nn{
namespace fs{
namespace{
    nn::Handle sFileServerSession = INVALID_HANDLE_VALUE;
    detail::FileSystemBase sFileSystemBase;
    detail::FileSystemBaseImpl sFileSystemBaseImpl;

    nn::srv::LightEventNotificationHandler sNotificationCardInsertedHandler;
    nn::srv::LightEventNotificationHandler sNotificationCardEjectedHandler;
    nn::srv::LightEventNotificationHandler sNotificationSdmcInsertedHandler;
    nn::srv::LightEventNotificationHandler sNotificationSdmcEjectedHandler;
}

namespace detail{
    inline ipc::FileSystem GetIpcFileSystem(){
        Result res;
        if(fs::sFileServerSession.IsValid())
            NN_ERR_THROW_FATAL_ALL(res);
        return ipc::FileSystem(sFileServerSession);
    }
}

bool IsInitialized(){
    return sFileServerSession.IsValid();
}

inline Result SetPriority(s32 pri){
    ipc::FileSystem sys = detail::GetIpcFileSystem();
    Result res = sys.SetPriority(pri);
}

void Initialize(){
    if(!IsInitialized()){
        Result res = srv::Initialize();
        if(res != nn::srv::ResultAlreadyInitialized())
            NN_ERR_THROW_FATAL_ALL(res);
        NN_ERR_THROW_FATAL_ALL(srv::GetServiceHandle(&sFileServerSession, detail::PORT_NAME_USER));
        sFileSystemBaseImpl.Initialize(sFileServerSession);
        sFileSystemBase.Initialize(sFileSystemBaseImpl);
        detail::RegisterGlobalFileSystemBase(sFileSystemBase);
        #if NN_VERSION_MAJOR > 2
            NN_ERR_THROW_FATAL_ALL(SetPriority(0));
        #endif
    }
}

void RegisterSdmcEjectedEvent(os::LightEvent* p){
    sNotificationSdmcEjectedHandler.Initialize(p);
    NN_ERR_THROW_FATAL_ALL(nn::srv::RegisterNotificationHandler(&sNotificationSdmcEjectedHandler, 521));
    NN_ERR_THROW_FATAL_ALL(nn::srv::Subscribe(521));
}

void UnregisterCardEjectedEvent(){
    NN_ERR_THROW_FATAL_ALL(nn::srv::Unsubscribe(521));
    nn::srv::UnregisterNotificationHandler(521);
}

bool IsSdmcInserted(){
    bool bInserted;
    NN_ERR_THROW_FATAL_ALL(nn::fs::detail::GetIpcFileSystem().IsSdmcDetected(&bInserted));
    return bInserted;
}

bool IsSdmcWritable(){
    bool isWritable;
    NN_ERR_THROW_FATAL_ALL(nn::fs::detail::GetIpcFileSystem().IsSdmcWritable(&isWritable));
    return isWritable;
}

}
}
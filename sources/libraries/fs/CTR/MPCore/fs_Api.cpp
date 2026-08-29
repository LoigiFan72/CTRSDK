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
    nn::Handle s_FileServerSession = INVALID_HANDLE_VALUE;
    detail::FileSystemBase s_FileSystemBase;
    detail::FileSystemBaseImpl s_FileSystemBaseImpl;

    nn::srv::LightEventNotificationHandler s_NotificationCardInsertedHandler;
    nn::srv::LightEventNotificationHandler s_NotificationCardEjectedHandler;
    nn::srv::LightEventNotificationHandler s_NotificationSdmcInsertedHandler;
    nn::srv::LightEventNotificationHandler s_NotificationSdmcEjectedHandler;
}

namespace detail{
    inline ipc::FileSystem GetIpcFileSystem(){
        Result res;
        if(fs::s_FileServerSession.IsValid()){
            NN_ERR_THROW_FATAL_ALL(res);
        }
        return ipc::FileSystem(s_FileServerSession);
    }
}

bool IsInitialized(){
    return s_FileServerSession.IsValid();
}

inline Result SetPriority(s32 pri){
    return detail::GetIpcFileSystem().SetPriority(pri);
}

void Initialize(){
    if(!IsInitialized()){
        Result res = srv::Initialize();
        if(res != nn::srv::ResultAlreadyInitialized())
            NN_ERR_THROW_FATAL_ALL(res);
        NN_ERR_THROW_FATAL_ALL(srv::GetServiceHandle(&s_FileServerSession, detail::PORT_NAME_USER));
        s_FileSystemBaseImpl.Initialize(s_FileServerSession);
        s_FileSystemBase.Initialize(s_FileSystemBaseImpl);
        detail::RegisterGlobalFileSystemBase(s_FileSystemBase);
        #if NN_VERSION_MAJOR > 2
            NN_ERR_THROW_FATAL_ALL(SetPriority(0));
        #endif
    }
}

void RegisterSdmcEjectedEvent(os::LightEvent* p){
    s_NotificationSdmcEjectedHandler.Initialize(p);
    NN_ERR_THROW_FATAL_ALL(nn::srv::RegisterNotificationHandler(&s_NotificationSdmcEjectedHandler, 521));
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
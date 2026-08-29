// Filename: fs_FileSystemBase.cpp
//
// Project: Horizon

#include <nn/fs/fs_FileSystemBase.h>

namespace nn{
namespace fs{
namespace detail{

static detail::FileSystemBase* s_pGlobalFileSystemBase;

void RegisterGlobalFileSystemBase(detail::FileSystemBase& base){
    s_pGlobalFileSystemBase = &base;
}

FileSystemBase& GetGlobalFileSystemBase(){
    if(!s_pGlobalFileSystemBase){
        NN_UTIL_PANIC_IF_FAILED(ResultLibraryNotInitialized());
    }

    NN_TASSERT_(s_pGlobalFileSystemBase);
    return *s_pGlobalFileSystemBase;
}

}
}
}
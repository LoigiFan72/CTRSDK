// Filename: fs_FileSystemBase.cpp
//
// Project: Horizon

#include <nn/fs/fs_FileSystemBase.h>

namespace nn{
namespace fs{
namespace detail{

static detail::FileSystemBase* spGlobalFileSystemBase;

void RegisterGlobalFileSystemBase(detail::FileSystemBase& base){
    spGlobalFileSystemBase = &base;
}
}
}
}
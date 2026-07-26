#pragma once

#include <nn/fs/CTR/MPCore/fs_UserFileSystem.h>
#include <nn/fs/fs_Result.h>

namespace nn {
namespace fs {
namespace CTR {
namespace MPCore {
namespace detail{

typedef nn::fslow::LowPath<const char*, const wchar_t*> Path;

class IFile{
public:
    virtual Result TryRead(s32* pOut, s64 offset, void* buffer, size_t size) = 0; // 0x0
    virtual Result TryWrite(s32* pOut, s64 offset, const void* buffer, size_t size, bool flush=true) = 0; // 0x4
    virtual Result TryGetSize(s64* pOut) const = 0; // 0x8
    virtual Result TrySetSize(s64 size) = 0; // 0xC
    virtual Result TryFlush() = 0; // 0x10

    virtual Result TrySetPriority(s32 priority) = 0; // 0x14
    virtual Result TryGetPriority(s32* pOut) const = 0; // 0x18

    virtual Result DuplicateHandle(Handle* pOut, s64 offset, s64 length) = 0; // 0x1C
    virtual Result OpenLinkHandle(Handle* pOut) = 0; // 0x20

    virtual Handle GetFileHandle() { return nn::Handle(); } // 0x24    
    virtual void DetachFileHandle() { } // 0x28

    virtual void Close() = 0; // 0x2C
    virtual ~IFile() {} // 0x30
};

class IDirectory{
public:
    virtual Result TryRead(s32* pNumEntriesOut, DirectoryEntry pEntries[], s32 numEntries) = 0;
    virtual void Close() = 0;

    virtual Result TrySetPriority(s32 priority) = 0;
    virtual Result TryGetPriority(s32* pOut) const = 0;
    
    virtual ~IDirectory() {}
};

class IArchive{
public:
    virtual Result OpenFile (IFile**, const Path&, bit32) = 0; // 0x0
    virtual Result OpenDirectory (IDirectory**, const Path&) = 0; // 0x4

    virtual Result DeleteFile (const Path&) = 0; // 0x8
    virtual Result RenameFile (const Path&, const Path&) = 0; //0xC

    virtual Result DeleteDirectory (const Path&) = 0; // 0x10
    virtual Result DeleteDirectoryRecursively (const Path&) = 0; // 0x14

    virtual Result CreateFile (const Path&, s64 size) = 0; // 0x18
    virtual Result CreateDirectory (const Path&) = 0; // 0x1C

    virtual Result SetArchivePriority(s32) = 0; // 0x20
    virtual Result GetArchivePriority(s32*) = 0; // 0x24

    virtual Result RenameDirectory (const Path&, const Path&) = 0; // 0x28

    virtual Result GetFreeBytes(s64*) { return fs::ResultUnsupportedOperation(); } // 0x2C

    virtual void DeleteObject() = 0; // 0x30
    virtual ~IArchive () {} // 0x34
};


} // namespace detail
} // namespace MPCore
} // namespace CTR
} // namespace fs
} // namespace nn
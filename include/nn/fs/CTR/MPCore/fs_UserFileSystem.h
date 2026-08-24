#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/fs/CTR/fs_PathNamesForSystem.h>
#include <nn/fs/fs_Parameters.h>
#include <nn/fs/fs_Result.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/util/util_Result.h>

#include <nn/os.h>
#include <nn/fnd.h>

#include <nn/fslow/fslow_Path.h>

namespace nn{
namespace fs{
namespace CTR{
namespace MPCore{

typedef nn::fslow::LowPath<const char*, const wchar_t*> Path;

namespace detail{

void LatencyEmulation(bool isRead);

class UserFileSystem{
public:
    /* Creating */

    static Result TryCreateFile(const wchar_t *pathName, s64 size);
    static Result TryCreateDirectory(const wchar_t *pathName);

    /* Open / Close Generaal */
    static Result Initialize(nn::Handle handle);
    static void CloseFile(void* p);
    static void CloseDirectory(void* p);

    static Result TryOpenFile(void** pout, const wchar_t* pathName,bit32 mode);
    static Result TryOpenDirectory(void** pOut, const wchar_t* pathName);

    static Result TryReadFile(s32* pOut, void* p, s64 offset, void* buffer, size_t size);
    static Result TryReadDirectory(s32* pOut, void* p, DirectoryEntry pEntries[], s32 numEntries);

    static Result TryWriteFile(s32* pOut, void* p, s64 offset, const void* buffer, size_t size, bool flush);

    static Result TryGetFileSize(s64* pout,void*);
    static Result TrySetFileSize(void* p, s64 size);

    static Result TryFlush(void*);

    /* Delete */

    static Result TryDeleteFile(const wchar_t *pathName);

};

}
}
}
}
}
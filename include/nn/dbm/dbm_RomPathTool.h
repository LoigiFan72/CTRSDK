#pragma once

#include <nn/Result.h>
#include <nn/dbm/dbm_Parameters.h>
#include <nn/dbm/dbm_Result.h>
#include <wchar.h>

namespace nn{
namespace dbm{
namespace RomPathTool{

static const u32 MAX_PATH_LENGTH = 256;

struct RomEntryName{
    size_t length;
    const RomPathChar* path;
};

inline bool IsSeparator(RomPathChar ch){
    return (ch == '/');
}

    /* Current Dir */

inline bool IsCurrentDirectory(const RomEntryName& dirName){
    return ((dirName.length == 1) && (dirName.path[0] == L'.'));
}
inline bool IsCurrentDirectory(const RomPathChar* pDirName){
    return ((pDirName[0] == L'.') && (pDirName[1] == NULL));
}
inline bool IsCurrentDirectory(const RomPathChar* pDirName, size_t length){
    return ((length == 1) && (pDirName[0] == L'.'));
}

/* Parent Directories */

inline bool IsParentDirectory(const RomEntryName& dirName){
    return ((dirName.length == 2) && (dirName.path[0] == L'.') && (dirName.path[1] == L'.'));
}
inline bool IsParentDirectory(const RomPathChar* pDirName){
    return ((pDirName[0] == L'.') && (pDirName[1] == L'.') && (pDirName[2] == NULL));
}
inline bool IsParentDirectory(const RomPathChar* pDirName, size_t length){
    return ((length == 2) && (pDirName[0] == L'.') && (pDirName[1] == L'.'));
}

inline bool IsEqualPath(const RomPathChar* pExtraKey1, const RomPathChar* pExtraKey2, size_t length){
    return (wcsncmp(pExtraKey1, pExtraKey2, length) == 0);
}

Result GetParentDirectoryName(RomEntryName* pOut, const RomEntryName& base, const RomPathChar* pHead);

class PathParser{
protected:
    const RomPathChar* m_pPrevStartPath;
    const RomPathChar* m_pPrevEndPath;
    const RomPathChar* m_pNextPath;
    bool m_bParseFinished;
    short rev;

public:
    PathParser();
    Result Initialize(const RomPathChar* pFullPath);
    bool IsParseFinished() const;
    bool IsDirectoryPath() const;
    Result GetNextDirectoryName(RomEntryName* pDirName);
    Result GetAsDirectoryName(RomEntryName* pName) const;
    Result GetAsFileName(RomEntryName* pName) const;
};
}
}
}
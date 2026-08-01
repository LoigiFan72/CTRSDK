// Filename: dbm_RomPathTool.cpp
//
// Project: Horizon

#include <nn/dbm/dbm_RomPathTool.h>
#include <nn/Assert.h>

namespace nn{
namespace dbm{
namespace RomPathTool{

/* PathParser*/

PathParser::PathParser(): 
    mpPrevStartPath(NULL),
    mpPrevEndPath(NULL),
    mpNextPath(NULL),
    mbParseFinished(false)
{}

Result RomPathTool::PathParser::GetAsDirectoryName(RomEntryName* pName) const{
    NN_NULL_TASSERT_(this->mpNextPath);
    NN_NULL_TASSERT_(this->mpPrevStartPath);
    NN_NULL_TASSERT_(this->mpPrevEndPath);
    NN_NULL_TASSERT_(pName);

    size_t numChar = mpPrevEndPath - mpPrevStartPath;
    if (numChar > MAX_PATH_LENGTH){
        return ResultDirectoryNameTooLong();
    }

    pName->length = numChar;
    pName->path = mpPrevStartPath;

    return ResultSuccess();
}

Result RomPathTool::PathParser::GetAsFileName(RomEntryName* pName) const{
    NN_NULL_TASSERT_(this->mpNextPath);
    NN_NULL_TASSERT_(this->mpPrevStartPath);
    NN_NULL_TASSERT_(this->mpPrevEndPath);
    NN_NULL_TASSERT_(pName);

    size_t numChar = mpPrevEndPath - mpPrevStartPath;
    if (numChar > MAX_PATH_LENGTH){
        return ResultFileNameTooLong();
    }

    pName->length = numChar;
    pName->path = mpPrevStartPath;

    return ResultSuccess();
}

Result PathParser::Initialize(const RomPathChar* pFullPath){
    NN_NULL_TASSERT_(pFullPath);

    if (! IsSeparator(pFullPath[0])){
        return ResultInvalidPathFormat();
    }

    while (IsSeparator(pFullPath[1])){
        pFullPath ++;
    }

    mpPrevStartPath = pFullPath;

    mpPrevEndPath = mpPrevStartPath;

    mpNextPath = &pFullPath[1];
    while (IsSeparator(this->mpNextPath[0])){
        this->mpNextPath ++;
    }

    return ResultSuccess();
}

Result PathParser::GetNextDirectoryName(RomEntryName* pDirName){
    NN_NULL_TASSERT_(mpPrevStartPath);
    NN_NULL_TASSERT_(mpPrevEndPath);
    NN_NULL_TASSERT_(mpNextPath);
    NN_NULL_TASSERT_(pDirName);

    pDirName->length = (mpPrevEndPath - mpPrevStartPath);
    pDirName->path = mpPrevStartPath;

    mpPrevStartPath = mpNextPath;

    const RomPathChar* p = mpNextPath;
    for (size_t dirNameLength = 0; ; dirNameLength++){
        if (IsSeparator(p[dirNameLength])){
            if (dirNameLength >= MAX_PATH_LENGTH){
                return ResultDirectoryNameTooLong();
            }

            mpPrevEndPath = &p[dirNameLength];
            mpNextPath = mpPrevEndPath + 1;

            while (IsSeparator(*this->mpNextPath)){
                mpNextPath ++;
            }

            if (*this->mpNextPath == NULL){
                mbParseFinished = true;
            }
            break;
        }

        if (p[dirNameLength] == NULL){
            mbParseFinished = true;
            mpPrevEndPath = mpNextPath = &p[dirNameLength];
            break;
        }
    }

    return ResultSuccess();
}

bool PathParser::IsParseFinished() const{
    return mbParseFinished;
}

bool RomPathTool::PathParser::IsDirectoryPath() const{
    NN_NULL_TASSERT_(mpNextPath);
    if ((mpNextPath[0] == NULL) && (mpNextPath[-1] == 0x2F)){
        return true;
    }

    if (IsCurrentDirectory(this->mpNextPath)){
        return true;
    }

    if (IsParentDirectory(this->mpNextPath)){
        return true;
    }

    return false;
}

Result GetParentDirectoryName(RomEntryName* pOut, const RomEntryName& base, const RomPathChar* pHead){
    const RomPathChar* pStart = base.path;
    const RomPathChar* pEnd = base.path + base.length - 1;

    s32 depth = 1;

    if (IsParentDirectory(base)){
        depth++;
    }
    
    if (base.path > pHead){
        size_t length = 0;
        const RomPathChar* p = base.path - 1;
        while (p >= pHead){
            if (IsSeparator(*p)){

                if (IsCurrentDirectory(p + 1, length)){
                    depth++;
                }

                if (IsParentDirectory(p + 1, length)){
                    depth += 2;
                }

                if (depth == 0){
                    pStart = p + 1;
                    break;
                }

                while (IsSeparator(*p)){
                    p--;
                }

                pEnd = p;
                length = 0;
                depth--;
            }
    
            length++;
            p--;
        }

        if (depth != 0){
            return ResultInvalidPathFormat();
        }

        if (p == pHead){
            pStart = pHead + 1;
        }
    }

    if (pEnd <= pHead){
        pOut->path = pHead;
        pOut->length = 0;
    }
    
    else{
        pOut->path = pStart;
        pOut->length = (pEnd - pStart + 1);
    }

    return ResultSuccess();
}
}
}
}
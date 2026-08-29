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
    m_pPrevStartPath(NULL),
    m_pPrevEndPath(NULL),
    m_pNextPath(NULL),
    m_bParseFinished(false)
{}

Result RomPathTool::PathParser::GetAsDirectoryName(RomEntryName* pName) const{
    NN_NULL_TASSERT_(this->m_pNextPath);
    NN_NULL_TASSERT_(this->m_pPrevStartPath);
    NN_NULL_TASSERT_(this->m_pPrevEndPath);
    NN_NULL_TASSERT_(pName);

    size_t numChar = m_pPrevEndPath - m_pPrevStartPath;
    if (numChar > MAX_PATH_LENGTH){
        return ResultDirectoryNameTooLong();
    }

    pName->length = numChar;
    pName->path = m_pPrevStartPath;

    return ResultSuccess();
}

Result RomPathTool::PathParser::GetAsFileName(RomEntryName* pName) const{
    NN_NULL_TASSERT_(this->m_pNextPath);
    NN_NULL_TASSERT_(this->m_pPrevStartPath);
    NN_NULL_TASSERT_(this->m_pPrevEndPath);
    NN_NULL_TASSERT_(pName);

    size_t numChar = m_pPrevEndPath - m_pPrevStartPath;
    if (numChar > MAX_PATH_LENGTH){
        return ResultFileNameTooLong();
    }

    pName->length = numChar;
    pName->path = m_pPrevStartPath;

    return ResultSuccess();
}

Result PathParser::Initialize(const RomPathChar* pFullPath){
    NN_NULL_TASSERT_(pFullPath);

    if (!IsSeparator(pFullPath[0])){
        return ResultInvalidPathFormat();
    }

    while(IsSeparator(pFullPath[1])){
        pFullPath++;
    }

    m_pPrevStartPath = pFullPath;

    m_pPrevEndPath = m_pPrevStartPath;

    m_pNextPath = &pFullPath[1];
    while (IsSeparator(this->m_pNextPath[0])){
        this->m_pNextPath++;
    }

    return ResultSuccess();
}

Result PathParser::GetNextDirectoryName(RomEntryName* pDirName){
    NN_NULL_TASSERT_(m_pPrevStartPath);
    NN_NULL_TASSERT_(m_pPrevEndPath);
    NN_NULL_TASSERT_(m_pNextPath);
    NN_NULL_TASSERT_(pDirName);

    pDirName->length = (m_pPrevEndPath - m_pPrevStartPath);
    pDirName->path = m_pPrevStartPath;

    m_pPrevStartPath = m_pNextPath;

    const RomPathChar* p = m_pNextPath;
    for (size_t dirNameLength = 0; ; dirNameLength++){
        if (IsSeparator(p[dirNameLength])){
            if (dirNameLength >= MAX_PATH_LENGTH){
                return ResultDirectoryNameTooLong();
            }

            m_pPrevEndPath = &p[dirNameLength];
            m_pNextPath = m_pPrevEndPath + 1;

            while (IsSeparator(*this->m_pNextPath)){
                m_pNextPath ++;
            }

            if (*this->m_pNextPath == NULL){
                m_bParseFinished = true;
            }
            break;
        }

        if (p[dirNameLength] == NULL){
            m_bParseFinished = true;
            m_pPrevEndPath = m_pNextPath = &p[dirNameLength];
            break;
        }
    }

    return ResultSuccess();
}

bool PathParser::IsParseFinished() const{
    return m_bParseFinished;
}

bool RomPathTool::PathParser::IsDirectoryPath() const{
    NN_NULL_TASSERT_(m_pNextPath);
    if ((m_pNextPath[0] == NULL) && (m_pNextPath[-1] == 0x2F)){
        return true;
    }

    if (IsCurrentDirectory(this->m_pNextPath)){
        return true;
    }

    if (IsParentDirectory(this->m_pNextPath)){
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
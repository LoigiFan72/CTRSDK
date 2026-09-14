#pragma once

#include <cwchar>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <cwchar>

namespace nn {
namespace fslow {

template <class T, typename V>
class LowPath
{
protected:
        bit32       m_PathType; // todo: possible enum?
        const void* m_Data;
        size_t      m_BinarySize;
public:
    LowPath (){
        this->m_PathType   = 4;
        this->m_Data       = &m_Data;
        this->m_BinarySize = 1;
    }
    LowPath (const wchar_t* path){
        this->m_PathType   = 4;
        this->m_Data       = path;
        this->m_BinarySize = 2 * (wcslen (path) + 1);
    }

    const wchar_t* GetWStringRaw() const
    {
        if (GetPathType () == 4) 
        {
            return static_cast<const wchar_t*>(m_Data);
        }
        return NULL;
    }
    bit32       GetPathType () const { return this->m_PathType; }
    const bit8* GetDataBuffer () const { return static_cast<const bit8*>(this->m_Data); }
    size_t      GetDataSize () const { return this->m_BinarySize; }

    template <typename T>
    void SetBinary (const T* p)
    {
        this->m_PathType   = 2;
        this->m_Data       = p;
        this->m_BinarySize = sizeof (T);
    }

    template <typename T>
    static LowPath Make(const T* p)
    {
        LowPath ret;
        ret.SetBinary(p);
        return ret;
    }
};

}
}
#pragma once

#include <cwchar>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <cwchar>

namespace nn {
namespace fslow {

template <class T, typename V>
class LowPath{
protected:
        bit32       mPathType; // todo: possible enum?
        const void* mData;
        size_t      mBinarySize;
public:
    LowPath (){
        this->mPathType   = 4;
        this->mData       = &mData;
        this->mBinarySize = 1;
    }
    LowPath (const wchar_t* path){
        this->mPathType   = 4;
        this->mData       = path;
        //this->mBinarySize = 2 * (wcslen (path) + 1);
    }

    const wchar_t* GetWStringRaw() const{
        if (GetPathType () == 4) {
            return static_cast<const wchar_t*>(mData);
        }
        return NULL;
    }
    bit32       GetPathType () const { return this->mPathType; }
    const bit8* GetDataBuffer () const { return static_cast<const bit8*>(this->mData); }
    size_t      GetDataSize () const { return this->mBinarySize; }

    template <typename T>
    void SetBinary (const T* p){
        this->mPathType   = 2;
        this->mData       = p;
        this->mBinarySize = sizeof (T);
    }

    template <typename T>
    static LowPath Make(const T* p){
        LowPath ret;
        ret.SetBinary(p);
        return ret;
    }
};

}
}
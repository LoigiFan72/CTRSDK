#pragma once

#include <nn/types.h>

namespace nn { 
namespace ro {

template <typename T>
class OffsetPointer{
private:
    uptr    mValue;
public:
    operator T*()                  { return &**this; }
    operator const T*() const      { return &**this; }
    operator uptr() const          { return mValue; }

    T* GetPointer(uptr baseAddr) const { return reinterpret_cast<T*>(mValue + baseAddr); }
    T* GetPointer(const void* baseAddr) const { return GetPointer(reinterpret_cast<uptr>(baseAddr)); }
    void SetPointer(uptr addr) { mValue = addr; }
    void SetPointer(int addr) { mValue = static_cast<uptr>(addr); }
    void SetPointer(void* addr) { mValue = reinterpret_cast<uptr>(addr); }
};

}
}
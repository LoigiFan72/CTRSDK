#pragma once

#include <nn/types.h>
#include <nn/Result.h>

namespace nn{
namespace ro{

class RegistrationList{
    uptr mDummy;
public:
    RegistrationList();
    ~RegistrationList(){ this->Unregister(); }
    Result Unregister();
    uptr GetHead() { return reinterpret_cast<uptr>(this); }

    void operator delete(void* p){ }
    void* operator new(size_t size) throw ();
};
}
}
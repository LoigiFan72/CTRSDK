#pragma once

namespace nn{
namespace os{

class AutoStackManager
{
public:
    virtual ~AutoStackManager() {}
    virtual void* Construct(size_t stackSize) = 0;
    virtual void Destruct(void* pStackBottom, bool isError) = 0;
};
    
}
}
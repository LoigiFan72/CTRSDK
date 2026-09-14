#pragma once

#include <nn/os/os_Event.h>

namespace nn{
namespace os{

class Semaphore : public InterruptEvent
{
public:
    Semaphore()
    { 
    }
    
    ~Semaphore()
    { 
    }

    void Acquire() 
    { 
        this->WaitOne(); 
    }
};

}
}
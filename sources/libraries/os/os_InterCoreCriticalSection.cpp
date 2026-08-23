// Filename: os_CriticalSection.cpp
//
// Project: Horizon
//
// Note: For [MAJOR] Version 2 and Below.

#pragma once

#include <nn/os/os_InterCoreCriticalSection.h>

namespace nn{ 
namespace os{

void InterCoreCriticalSection::EnterImpl(){
    for(;;){
        if(*mCounter > 0){
            if(this->TryEnterImpl()){
                break;
            }
        }

        this->mCounter.DecrementAndWaitIfLessThan(0);
    }
}

}
}
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
        if(*m_Counter > 0){
            if(this->TryEnterImpl()){
                break;
            }
        }

        this->m_Counter.DecrementAndWaitIfLessThan(0);
    }
}

}
}
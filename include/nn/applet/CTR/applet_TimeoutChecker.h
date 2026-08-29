#pragma once

#include <nn/os/os_Tick.h>
#include <nn/applet/CTR/applet_Paramaters.h>

namespace nn{
namespace applet{
namespace CTR{
namespace detail{

class TimeoutChecker{
    const fnd::TimeSpan m_Timeout;
    os::Tick m_Start;
public:
    TimeoutChecker(fnd::TimeSpan timeout):
        m_Timeout(timeout){
        if (timeout == CTR::NO_WAIT || timeout == CTR::WAIT_INFINITE) {
            m_Start = os::Tick();
        } 
        else {
            m_Start = os::Tick::GetSystemCurrent();
        }
    }
    ~TimeoutChecker(){ }
    
    bool Check(){
        if (m_Timeout == CTR::NO_WAIT){
            return true;
        }
        if (m_Timeout == CTR::WAIT_INFINITE){
            return false;
        }
        if (m_Timeout < nn::fnd::TimeSpan(nn::os::Tick::GetSystemCurrent() - this->m_Start)){
            return true;
        }
    }
};

}
}
}
}

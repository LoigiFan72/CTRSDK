#pragma once

#include <nn/os/os_SimpleLock.h>

namespace nn{
namespace os{

class LightEvent : private util::ADLFireWall::NonCopyable<LightEvent>{
private:
    WaitableCounter m_Counter;
    #if NN_VERSION_MAJOR > 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)
        SimpleLock m_Lock;
    #endif
public:

    static const s32 NOT_RESETED_MANUAL = -2;
    static const s32 NOT_RESETED_AUTO = -1;
    static const s32 RESETED_AUTO = 0;
    static const s32 RESETED_MANUAL = 1;

    LightEvent(){ }
    explicit LightEvent(bool isManuelReset){ this->Initialize(isManuelReset); }
    ~LightEvent(){ };

#if NN_VERSION_MAJOR > 2 ||  (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) ||  (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)
    void Initialize(bool);
#else
    void Initialize(bool isManualReset){ 
        *m_Counter = isManualReset ? NOT_RESETED_MANUAL: NOT_RESETED_AUTO;
    }
#endif
    void Finalize(){ }
    void ClearSignal();
    void Wait();
    void Signal();
    bool TryWait();

    bool IsSignaled() const { return *this->m_Counter >= 0; }
};

}
}
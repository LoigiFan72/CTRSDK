#pragma once

#include <nn/os/os_SimpleLock.h>

namespace nn{
namespace os{

class LightEvent : private nn::util::ADLFireWall::NonCopyable<LightEvent>{
private:
    nn::os::WaitableCounter mCounter;
    nn::os::SimpleLock mLock;
public:

    static const s32 NOT_RESETED_MANUAL = -2;
    static const s32 NOT_RESETED_AUTO = -1;
    static const s32 RESETED_AUTO = 0;
    static const s32 RESETED_MANUAL = 1;

    LightEvent(){ }
    explicit LightEvent(bool isManuelReset){ Initialize(isManuelReset); }
    ~LightEvent(){ };

    void Initialize(bool);
    void Finalize(){ }
    void ClearSignal();
    void Wait();
    void Signal();
    bool TryWait();

    bool IsSignaled() const { return *this->mCounter >= 0; }
};

}
}
#pragma once

#include <nn/os/os_LightSemaphore.h>
#include <nn/os/ARM/os_MemoryBarrier.h>

namespace nn { 
namespace os {

#if NN_VERSION_MAJOR > 2
    typedef LightSemaphore InterCoreLightSemaphore;
#else

class InterCoreLightSemaphore : private util::ADLFireWall::NonCopyable<LightSemaphore>
{
public:
    static const s32 MAX_MAX_COUNT  = 0x7fff;

private:
    struct DecrementIfPositive
    {
        bool operator()(s32& x)
        {
            if(x > 0){
                --x;
                return true;
            }

            else{
                return false;
            }
        }
    };
    struct LimitedAdd
    {
        s32 max;
        s32 value;
        s32 beforeUpdate;

        bool operator()(s32& x)
        {
            beforeUpdate = x;

            if(x > max - value){
                x = max;
            }

            else{
                x += value;
            }

            return true;
        }
    };

    WaitableCounter m_Counter;
    fnd::InterlockedVariable<s16> m_NumWaiting;
    s16 m_Max;
public:
    InterCoreLightSemaphore() 
    {
    }
    InterCoreLightSemaphore(s32 initialCount, s32 maxCount) 
    {
        Initialize(initialCount, maxCount); 
    }
    InterCoreLightSemaphore(s32 initialCount)
    { 
        Initialize(initialCount); 
    }
    ~InterCoreLightSemaphore()
    { 
        Finalize(); 
    }

    void Initialize(s32 initialCount) 
    { 
        Initialize(initialCount, MAX_MAX_COUNT); 
    }

    void Initialize(s32 initialCount, s32 maxCount)
    {
        NN_MIN_TASSERT_(initialCount, 0);
        NN_MIN_TASSERT_(maxCount, 1);
        NN_MAX_TASSERT_(initialCount, maxCount);
        NN_MAX_TASSERT_(maxCount, MAX_MAX_COUNT);
        *m_Counter = initialCount;
        m_NumWaiting = 0;
        m_Max = maxCount;
    }

    void Finalize()
    {
    }

    bool TryAcquire()
    {
        DecrementIfPositive updater;
        bool ret = m_Counter->AtomicUpdateConditional(updater);
        ARM::DataSynchronizationBarrier();
        return ret;
    }

    void Acquire()
    {
        while(!this->TryAcquire())
        {
            ++m_NumWaiting;
            this->m_Counter.WaitIfLessThan(1);
            --m_NumWaiting;
        }
    }

    s32 Release(s32 releaseCount = 1);
};

}

#endif
}
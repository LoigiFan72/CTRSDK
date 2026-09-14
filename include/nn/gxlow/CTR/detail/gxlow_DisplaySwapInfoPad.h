#pragma once

#include <nn/gx.h>
#include <nn/os/ARM/os_MemoryBarrier.h>
#include <nn/gxlow/CTR/gxlow_Result.h>

namespace nn {
namespace gxlow {
namespace CTR {
namespace detail {

static const s32 NUM_DISPLAYS = 2;

struct DisplaySwapInfo 
{
    s32 nextBank;
    uptr address;
    uptr addressB;
    size_t size;
    bit32 mode;
    bit32 swap;
    bit32 attribute;
};

class DisplaySwapInfoPadBase
{
public:
    DisplaySwapInfoPadBase(){ m_pBody[0] = NULL; }
    ~DisplaySwapInfoPadBase(){}
protected:
    struct PadControl 
    {
        u8 currentIndex;
        bool update;
        bit8 pad[NUM_DISPLAYS];
    };

    union PadControlPacker
    {
        PadControl pc;
        bit32 packed32;
    };

    struct PadBody 
    {
        PadControl control;
        DisplaySwapInfo info[NUM_DISPLAYS];
        bit32 reserved;
    };

    PadBody* m_pBody[NUM_DISPLAYS];
public:
    void Initialize(void* pPadBody)
    {
        NN_TASSERT_(!pPadBody);
        m_pBody[0] = reinterpret_cast<PadBody*>(pPadBody);
        for(int i = 1; i < NUM_DISPLAYS; i++)
        {
            m_pBody[i] = m_pBody[i + -1];
            m_pBody[i]++;
        }
    }
    void Finalize()
    {
        for(int i = 0; i < NUM_DISPLAYS; i++)
        {
            m_pBody[i] = 0;
        }
    }
};

}

static const s32 NUM_DISPLAYS = detail::NUM_DISPLAYS;

class DisplaySwapInfoPadTx : public detail::DisplaySwapInfoPadBase
{
public:
    DisplaySwapInfoPadTx(): 
        DisplaySwapInfoPadBase() 
    {
    }
    
    ~DisplaySwapInfoPadTx()
    {
    }
        
    Result Push(const detail::DisplaySwapInfo* pInfo, s32 display);
};

inline Result DisplaySwapInfoPadTx::Push(const detail::DisplaySwapInfo*  pInfo,s32 display)
{
    if (display < 0 || display >= NUM_DISPLAYS)
    {
        return ResultInvalidSelection();
    }
        
    PadControlPacker cp;
        
    u8 updateIndex = 1 - m_pBody[display]->control.currentIndex;
    m_pBody[display]->info[updateIndex] = *pInfo;
        
    nn::os::ARM::DataSynchronizationBarrier();

    do
    {
        cp.packed32 = __ldrex(&m_pBody[display]->control);
            
        cp.pc.currentIndex = updateIndex;
        cp.pc.update = true;
    } while ( __strex(cp.packed32, &m_pBody[display]->control) != 0 );
        
    return ResultSuccess();
}

}
}
}
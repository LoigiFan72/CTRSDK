#pragma once

#include "nn/fnd/fnd_Intrusive.h"

namespace nn{
namespace os{
class MemoryBlockBase : public nn::fnd::IntrusiveLinkedList<MemoryBlockBase>::Item{
protected:
    void SetReadOnly(bool readOnly) {
        m_ReadOnly = readOnly; 
    }
public:
    uptr m_Addr;
    size_t m_Size;
    bool m_ReadOnly;
    s8 rev[3];

    void SetAddressAndSize(uptr addr, size_t size){
        m_Addr = addr;
        m_Size = size;
    }
    uptr GetAddress() const { return m_Addr; }
    size_t GetSize() const {return m_Size; }
    bool IsReadOnly() const {return m_ReadOnly; }
};

}
}


typedef union nnosMemoryBlockBase{
    
} nnosMemoryBlockBase;
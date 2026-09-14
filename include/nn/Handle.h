#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/WithInitialize.h>

typedef struct nnHandle{
    bit32 value;
} nnHandle;


namespace nn{
namespace{
    const nnHandle PSEUDO_HANDLE_CURRENT_THREAD  = {0xFFFF8000};
    const nnHandle PSEUDO_HANDLE_CURRENT_PROCESS = {0xFFFF8001};
    const nnHandle INVALID_HANDLE_VALUE = {0};
}
struct Handle {
public:
    Handle (): 
        m_Handle (0)
    {
    }

    Handle (nnHandle handle): 
        m_Handle (handle.value)
    {
    }

    Handle(const nn::WithoutInitialize&)
    {
    }

    Handle (bit32 value): 
        m_Handle (value)
    {
    }

    bool IsValid() const 
    {
        return m_Handle != 0;
    }

    bool operator==(int other) const 
    {
        return m_Handle == (u32)other;
    }
    bool operator!=(int other) const 
    {
        return m_Handle != (u32)other;
    }

    Handle& operator=(int other) 
    {
        m_Handle = (u32)other;
        return *this;
    }

    Handle& operator=(u32 other) 
    {
        m_Handle = other;
        return *this;
    }

    Handle& operator=(nn::Handle* other) 
    {
        m_Handle = (u32)other;
        return *this;
    }

    bool operator== (const Handle& rhs) const { return this->m_Handle == rhs.m_Handle; }
    bool operator!= (const Handle& rhs) const { return this->m_Handle != rhs.m_Handle; }
    operator nnHandle () const
    {
        nnHandle result = {m_Handle};
        return result;
    }

    bit32 m_Handle;

    bit32 GetPrintableBits()
    {
        return this->m_Handle;
    }
};
}
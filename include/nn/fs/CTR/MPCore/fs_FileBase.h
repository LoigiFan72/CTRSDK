#pragma once

#include <nn/fs/CTR/MPCore/fs_UserFileSystem.h>
#include <nn/Assert.h>
#include <nn/err/CTR/err_Api.h>
#include <nn/util/util_Int64.h>

using namespace nn::fs::CTR::MPCore::detail;

namespace nn{
namespace fs{
namespace detail{

class FileBaseImpl
{
protected:
    void* m_P;
private:
    void* GetPtr() const  
    {
        return reinterpret_cast<void*>(reinterpret_cast<uptr>(this->m_P) & ~static_cast<uptr>(0x1)); 
    }
public:
    FileBaseImpl() : m_P(0) {}

    void Finalize()
    {
        if(this->GetPtr())
        {
            NN_TPANIC_IF_FALSE_(!this->IsNotFlushed());
            UserFileSystem::CloseFile(GetPtr());
            m_P = 0;
        }
    }
    
    ~FileBaseImpl(){ this->Finalize(); }
        
    Result TryGetSize(s64* pOut) const 
    {
        return UserFileSystem::TryGetFileSize(pOut, GetPtr()); 
    }
        
    Result TryFlush()
    {
        this->DoneFlush();
        return UserFileSystem::TryFlush(GetPtr());
    }

    Result TryRead(s32* pOut, s64 offset, void* buf, size_t size){ return UserFileSystem::TryReadFile(pOut,this->GetPtr(),offset,buf,size); }
    
    Result TryWrite(s32* pOut, s64 offset, const void* buffer, size_t size, bool flush=true)
    {
        if (!flush)
        {
            this->NeedFlush();
        }
        else
        {
            this->DoneFlush();
        }
        return UserFileSystem::TryWriteFile(pOut, GetPtr(), offset, buffer, size, flush);
    }

    Result TryOpenImpl(const wchar_t* path, bit32 mode)
    {
        return UserFileSystem::TryOpenFile(&this->m_P, path, mode);
    }

    Result TrySetSize(s64 size) { return UserFileSystem::TrySetFileSize(GetPtr(), size); }
    
    void NeedFlush() { m_P = reinterpret_cast<void*>(reinterpret_cast<uptr>(m_P) | 0x1); }
    bool IsNotFlushed() { return (reinterpret_cast<uptr>(m_P) & 0x1) != 0; }
    void DoneFlush() { m_P = reinterpret_cast<void*>(reinterpret_cast<uptr>(m_P) & ~static_cast<uptr>(0x1)); }
};

}
}
}
#pragma once

#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/fs/fs_Parameters.h>

namespace nn{
namespace fs{
namespace ipc{

class File{
    nn::Handle m_Session;
public:
    File(Handle session): 
        m_Session(session) 
    {
    }
    Result OpenSubFile(Handle* pOut, s64 offset, s64 length);
    Result Read(int* pRead, s64 pOffset, void* pBuffer, size_t pSize);
    Result Write(int* pWritten, s64 pOffset, const void* pBuffer, size_t pSize, nn::fs::WriteOption pOption);
    Result GetSize(s64* pOut);
    Result SetSize(s64 pSize);
    Result Close();
    Result SetPriority(int pPriority);
    Result GetPriority(s32* pOut);
    Result OpenLinkFile(nn::Handle* pOut);
};

}
}
}
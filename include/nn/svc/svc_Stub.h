#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/os/os_Types.h>

namespace nn{
namespace svc{
    Result ControlMemory(uint*,uint,uint,uint,uint,uint);
    Result ExitProcess();
    Result QueryMemory(nn::os::MemoryInfo*,nn::os::PageInfo*,uint);
    Result CreateThread(nn::Handle*, void (*)(uint), uint, uint, int, int);
    Result ExitThread();
    Result SleepThread(long long);
    Result GetThreadPriority();
    Result CreateMutex(nn::Handle*,bool);
    Result CreateEvent(nn::Handle*,nn::os::ResetType);
    Result CreateAddressArbiter(nn::Handle*);
    Result CreateMemoryBlock(nn::Handle* pOut, uptr pMemory, size_t size, bit32 myPermission, bit32 otherPermission);
    Result ArbitrateAddress(nn::Handle,uint,nn::os::ArbitrationType,s32,s64);
    Result Break(nn::dbg::BreakReason,const void*,int);
    Result CloseHandle(nn::Handle);
    Result ConnectToPort(nn::Handle*, const char*);
    Result DuplicateHandle(nn::Handle*, Handle);
    Result GetProcessId(uint*, nn::Handle);
    Result GetResourceLimit(nn::Handle*, Handle);
    Result GetResourceLimitCurrentValues(s64 values[], nn::Handle resourceLimit, const nn::os::LimitableResource names[], s32 umNames);
    s64 GetSystemTick();
    Result GetThreadId(uint*,nn::Handle);
    Result WaitSynchronizationN(int*, const nn::Handle*, int,bool,long long);
    Result MapMemoryBlock(nn::Handle,uptr,bit32,uint);
    Result UnmapMemoryBlock(nn::Handle,uptr);
    Result OutputDebugString(const char* text, s32 length);
    Result ReleaseMutex(nn::Handle);
    Result SignalEvent(nn::Handle);
    Result ClearEvent(nn::Handle);
    Result SendSyncRequest(nn::Handle);
};
}
#pragma once

#include <nn/os/os_Alarm.h>
#include <nn/os/os_AutoStackManager.h>
#include <nn/os/os_BlockingQueue.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/os/os_Default.h>
#include <nn/os/os_Event.h>
#include <nn/os/os_Initialize.h>
#include <nn/os/os_LightEvent.h>
#include <nn/os/os_LightSemaphore.h>
#include <nn/os/os_LockPolicy.h>
#include <nn/os/os_Memory.h>
#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_MemoryBlockBase.h>
#include <nn/os/os_Mutex.h>
#include <nn/os/os_Result.h>
#include <nn/os/os_SharedMemory.h>
#include <nn/os/ARM/os_SpinWait.h>
#include <nn/os/os_StackMemory.h>
#include <nn/os/os_Semaphore.h>
#include <nn/os/os_Synchronization.h>
#include <nn/os/os_Thread.h>
#include <nn/os/os_ThreadLocalStorage.h>
//#include <nn/os/os_ThreadPool.h>
#include <nn/os/os_Tick.h>
#include <nn/os/os_TransferMemoryBlock.h>
#include <nn/os/os_Types.h>
#include <nn/os/os_WaitableCounter.h>

#ifdef __cplusplus

using namespace nn::os;
using namespace nn::os::CTR::detail;

#endif
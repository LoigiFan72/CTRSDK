#pragma once

#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/os/CTR/os_ErrorHandler.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/os/CTR/MPCore/os_MemoryMap.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/os/os_SharedInfo.h>

#define NN_OS_WAIT_INFINITE -1

namespace nn{
namespace os{

const s64 WAIT_INFINITE = NN_OS_WAIT_INFINITE;

    enum ArbitrationType{
        ARBITRATION_TYPE_SIGNAL = 0,
        ARBITRATION_TYPE_WAIT_IF_LESS_THAN = 1,
        ARBITRATION_TYPE_DECREMENT_AND_WAIT_IF_LESS_THAN = 2, // lonh ahh names
        ARBITRATION_TYPE_WAIT_IF_LESS_THAN_WITH_TIMEOUT = 3,
        ARBITRATION_TYPE_DECREMENT_AND_WAIT_IF_LESS_THAN_WITH_TIMEOUT = 4,
        ARBITRATION_TYPE_MAX_BIT = 0x80000000,
    };

    enum ResetType{
        RESET_TYPE_ONESHOT = 0,
        RESET_TYPE_STICKY = 1,
        RESET_TYPE_PULSE = 2,
        RESET_TYPE_MAX_BIT = 0x80000000,
    };

    struct PageInfo{
        bit32 flags;
    };

    enum HandleInfoType{
        HANDLE_INFO_TYPE_CREATION_TIME = 0,
        HANDLE_INFO_TYPE_REFERENCE_COUNT = 1,
        HANDLE_INFO_TYPE_PARENT_PROCESS_ID = 2,
        HANDLE_INFO_TYPE_MAX_BIT = 0x80000000,
    };

    enum MemoryState{
        MEMORY_STATE_FREE = 0,
        MEMORY_STATE_RESERVED = 1,
        MEMORY_STATE_IO = 2,
        MEMORY_STATE_STATIC = 3,
        MEMORY_STATE_CODE = 4,
        MEMORY_STATE_PRIVATE = 5,
        MEMORY_STATE_SHARED = 6,
        MEMORY_STATE_CONTINUOUS = 7,
        MEMORY_STATE_ALIASED = 8,
        MEMORY_STATE_ALIAS = 9,
        MEMORY_STATE_ALIAS_CODE = 10,
        MEMORY_STATE_LOCKED = 11,
        MEMORY_STATE_MAX_BITS = 0x80000000,
    };

    enum LimitableResource{
        LIMITABLE_RESOURCE_MAX_PRIORITY = 0,
        LIMITABLE_RESOURCE_MAX_COMMIT = 1,
        LIMITABLE_RESOURCE_MAX_THREAD = 2,
        LIMITABLE_RESOURCE_MAX_EVENT = 3,
        LIMITABLE_RESOURCE_MAX_MUTEX = 4,
        LIMITABLE_RESOURCE_MAX_SEMAPHORE = 5,
        LIMITABLE_RESOURCE_MAX_TIMER = 6,
        LIMITABLE_RESOURCE_MAX_SHAREDMEMORY = 7,
        LIMITABLE_RESOURCE_MAX_ADDRESSARBITER = 8,
        LIMITABLE_RESOURCE_MAX_CPUTIME = 9,
        LIMITABLE_RESOURCE_END = 10,
        LIMITABLE_RESOURCE_MAX_BIT = 0x80000000,
    };

    enum ProcessInfoType{
        PROCESS_INFO_TYPE_USING_PHYSICAL_MEMORY_SIZE = 0,
        PROCESS_INFO_TYPE_MAX_USING_PHYSICAL_MEMORY_SIZE = 1,
        PROCESS_INFO_TYPE_PRIVATE_PHYSICAL_MEMORY_SIZE = 2,
        PROCESS_INFO_TYPE_MAX_RPIVATE_PHYSICAL_MEMORY_SIZE = 3,
        PROCESS_INFO_TYPE_NUM_HANDLES = 4,
        PROCESS_INFO_TYPE_MAX_NUM_HANDLES  = 5,
        PROCESS_INFO_TYPE_NUM_CREATED_THREADS = 6,
        PROCESS_INFO_TYPE_NUM_THREADS = 7,
        PROCESS_INFO_TYPE_MAX_NUM_THREADS = 8,
        PROCESS_INFO_TYPE_CPU_TIME_SYSTEM = 9,
        PROCESS_INFO_TYPE_CPU_TIME_USER = 10,
        PROCESS_INFO_TYPE_CPU_TIME = 11,
        PROCESS_INFO_TYPE_NUM_THREAD_SWITCH = 12,
        PROCESS_INFO_TYPE_NUM_FPU_SWITCH = 13,
        PROCESS_INFO_TYPE_NUM_PROCESS_SWITCH = 14,
        PROCESS_INFO_TYPE_NUM_SYSTEM_CALL = 15,
        PROCESS_INFO_TYPE_NUM_IPC = 16,
        PROCESS_INFO_TYPE_UPTIME = 17,
        PROCESS_INFO_TYPE_NAME = 18,
        PROCESS_INFO_TYPE_REGION = 19,
        PROCESS_INFO_TYPE_DEVICE_ADDRESS_OFFSET = 20,
        PROCESS_INFO_TYPE_WRAM_ADDRESS_OFFSET = 21,
        PROCESS_INFO_TYPE_WRAM_ADDRESS = 22,
        PROCESS_INFO_TYPE_WRAM_SIZE = 23,
        PROCESS_INFO_TYPE_MAX_BIT = 2147483648,
    };

    enum MemoryPermission{
        MEMORY_PERMISSION_NONE          = 0,
        MEMORY_PERMISSION_READ          = (1u <<  0),
        MEMORY_PERMISSION_WRITE         = (1u <<  1),
        MEMORY_PERMISSION_READ_WRITE    = (MEMORY_PERMISSION_READ | MEMORY_PERMISSION_WRITE),

        MEMORY_PERMISSION_DONT_CARE     = (1u << 28),
        MEMORY_PERMISSION_MAX_BITS      = (1u << 31)
    };

    enum MemoryOperation{
        MEMORY_OPERATION_FREE           = (1 << 0),
        MEMORY_OPERATION_RESERVE        = (2 << 0),
        MEMORY_OPERATION_COMMIT         = (3 << 0),
        MEMORY_OPERATION_MAP            = (4 << 0),
        MEMORY_OPERATION_UNMAP          = (5 << 0),
        MEMORY_OPERATION_PROTECT        = (6 << 0),

        MEMORY_OPERATION_REGION_APP     = (1 << 8),
        MEMORY_OPERATION_REGION_SYSTEM  = (2 << 8),
        MEMORY_OPERATION_REGION_BASE    = (3 << 8),

        MEMORY_OPERATION_MASK           = (0xFFu <<  0),
        MEMORY_OPERATION_REGION_MASK    = (0xFu  <<  8),
        MEMORY_OPERATION_FLAG_LINEAR    = (0x1u  << 16),

        MEMORY_OPERATION_MAX_BITS       = (1u << 31)
    };

    struct MemoryInfo{
        uptr baseAddress;
        size_t size;
        bit32 permission;
        MemoryState state;
    };

    struct NonCopyable{};
}
}
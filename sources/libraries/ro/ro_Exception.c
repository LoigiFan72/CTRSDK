// Filename: ro_Exception.c
//
// Project: Horizon

#include <nn/Result.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

extern "C" void* nnRoDetailFindEitEntry(uptr addr);
extern "C" nnResult nnRoDetailInitializeImpl(void* pRs, size_t rsSize);

extern "C" nnResult nnRoDetailInitializeLinkException(void* pRs, size_t rsSize){
    return nnRoDetailInitializeImpl(pRs, rsSize);
}
#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/ro/ro_Types.h>

namespace nn {
namespace ro {
namespace detail {

class DynamicLoader{
public:
    DynamicLoader() {}

    static Handle sSession;

    static Result Startup(Handle process, uptr staticInfo, size_t staticInfoSize, uptr locateAddr);
    static Result RegisterList(Handle process, uptr rr, size_t rrSize);
    static Result UnregisterList(Handle process, uptr rr);
    static Result Load(size_t* pFixedSize, Handle process, uptr ro, uptr roRelocate, size_t roSize, uptr dataAddr, uptr dataRelocate, size_t dataSize, uptr bssAddr, uptr bssSize, bool doRegister, nn::ro::FixLevel fixLevel, uptr rr);
    static Result Unload(Handle process, uptr roModule, size_t roSize, uptr originalAddr);
    static Result Link(Handle process, uptr roModule);
    static Result Unlink(Handle process, uptr roModule);
    static Result Cleanup(Handle process, uptr originalAddr);
};

}
}
}
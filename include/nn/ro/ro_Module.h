#pragma once

#include <nn/Result.h>
#include <nn/ro/ro_Types.h>

namespace nn{
namespace ro{

class Module{
public:
    uptr mDummy;

    class EnumerateCallback{
    public:

        virtual bool operator()(Module* p) = 0;
    };

    ~Module();
    Result Unload();
    Result Link();
    Result Unlink();
    bool IsAllSymbolResolved() const;
    const char* GetName() const;
    void DoInitialize();
    void DoFinalize();
    void GetRegionInfo(nn::ro::RegionInfo* pRegionInfo);
    static void Enumerate(nn::ro::Module::EnumerateCallback* p);
    static Module* Find(const char* pName);
    uptr GetAddress(const char* name) const;
    uptr GetAddress(s32 index) const;

private:
    Module();
    uptr GetHead() { return reinterpret_cast<uptr>(this); }

};
}
}
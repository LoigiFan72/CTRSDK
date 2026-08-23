// Filename: [MPCORE] fs_AccessAnalysis.cpp
//
// Project: Horizon

#include "fs_AccessAnalysis.h"

#include <string>
#include <nn/os.h>
#include <nn/dbg.h>
#include <nn/Assert.h>

namespace nn{
namespace fs{
namespace analysis{

#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)

bool sAnalysisLog = false;
nn::os::CriticalSection sAnalysisLogLock = nn::os::CriticalSection(nn::WithInitialize());

bool IsAnalysisLogEnabled(){
    return sAnalysisLog;
} 

void FsAnalysisLog(nn::Result result, nn::os::Tick tickStart, const char* fmt, ...){
    NN_TASSERT_(nn::fs::analysis::IsAnalysisLogEnabled());

    nn::os::CriticalSection::ScopedLock lock(sAnalysisLogLock);

    static char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ::std::va_list va;
    va_start(va, fmt);
    std::vsnprintf(buffer, sizeof(buffer), fmt, va);
    va_end(va);

    static char buffer2[1024];
    std::memset(buffer2, 0, sizeof(buffer2));
    nn::os::Tick tickEnd = nn::os::Tick::GetSystemCurrent();
    std::snprintf(buffer2,sizeof(buffer2),"[FS_ANALYSIS] %9lld-%9lld[ms]: %s,Result=%08X\n",(s64)tickStart * 1000 / nn::os::Tick::TICKS_PER_SECOND,
        (s64)tickEnd * 1000 / nn::os::Tick::TICKS_PER_SECOND,buffer,result.GetPrintableBits());

    nn::dbg::detail::PutString(buffer2, std::strlen(buffer2));
    }

#else

void FsAnalysisLog(nn::Result, nn::os::Tick, const char*, ...) {}

#endif
}
}
}
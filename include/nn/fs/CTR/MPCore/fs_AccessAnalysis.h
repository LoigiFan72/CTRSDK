#pragma once

#include <cstdio>
#include <nn/Result.h>
#include <nn/os/os_CriticalSection.h>

namespace nn{
namespace fs{
namespace analysis{

#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)

bool IsAnalysisLogEnabled();
void FsAnalysisLog(nn::Result result, nn::os::Tick tickStart, const char* fmt, ...);

#define NN_FS_ANALYSIS_LOG(fmt, ...)                                                             \
    if (nn::fs::analysis::IsAnalysisLogEnabled())                                             \
    {                                                                                         \
        nn::os::Tick tickLogStart;                                                            \
        tickLogStart = nn::os::Tick::GetSystemCurrent();                                      \
        nn::fs::analysis::FsAnalysisLog(nn::ResultSuccess(), tickLogStart, fmt, __VA_ARGS__); \
    }                                                                                         \

#define NN_FS_ANALYSIS_LOG_RETURN(result, fmt, ...)                                         \
    do                                                                                   \
    {                                                                                    \
        bool isLogEnabled = nn::fs::analysis::IsAnalysisLogEnabled();                    \
        nn::os::Tick tickLogStart;                                                       \
        if (isLogEnabled)                                                                \
        {                                                                                \
            tickLogStart = nn::os::Tick::GetSystemCurrent();                             \
            nn::Result resultTmp_ = (result);                                            \
            nn::fs::analysis::FsAnalysisLog(resultTmp_, tickLogStart, fmt, __VA_ARGS__); \
            return resultTmp_;                                                           \
        }                                                                                \
        return (result);                                                                 \
    }                                                                                    \
    while (0)

#define NN_FS_ANALYSIS_LOG_NORETURN(expr, fmt, ...)                                                  \
    do                                                                                            \
    {                                                                                             \
        bool isLogEnabled = nn::fs::analysis::IsAnalysisLogEnabled();                             \
        nn::os::Tick tickLogStart;                                                                \
        if (isLogEnabled)                                                                         \
        {                                                                                         \
            tickLogStart = nn::os::Tick::GetSystemCurrent();                                      \
            (expr);                                                                               \
            nn::fs::analysis::FsAnalysisLog(nn::ResultSuccess(), tickLogStart, fmt, __VA_ARGS__); \
            return;                                                                               \
        }                                                                                         \
        (expr);                                                                                   \
    }                                                                                             \
    while (0)

#define NN_FS_ANALYSIS_LOG_INIT_TICK()                                             \
    bool _isLogEnabled = nn::fs::analysis::IsAnalysisLogEnabled();              \
    nn::os::Tick _tickLogStart;                                                 \
    if (_isLogEnabled)                                                          \
    {                                                                           \
        _tickLogStart = nn::os::Tick::GetSystemCurrent();                       \
    }

#define NN_FS_ANALYSIS_LOG_RETURN_WITHOUT_TICK(result, fmt, ...)                             \
    do                                                                                    \
    {                                                                                     \
        nn::Result resultTmp_ = (result);                                                 \
        if (_isLogEnabled)                                                                \
        {                                                                                 \
            nn::fs::analysis::FsAnalysisLog(resultTmp_, _tickLogStart, fmt, __VA_ARGS__); \
        }                                                                                 \
        return resultTmp_;                                                                \
    }                                                                                     \
    while (0)

#else

#define NN_FS_ANALYSIS_LOG(fmt, ...) (void)0
#define NN_FS_ANALYSIS_LOG_RETURN(result, fmt, ...) do { return (result); }while(0)
#define NN_FS_ANALYSIS_LOG_NORETURN(expr, fmt, ...) do { (expr); }while(0)
#define NN_FS_ANALYSIS_LOG_INIT_TICK() (void)0
#define NN_FS_ANALYSIS_LOG_RETURN_WITHOUT_TICK(result, fmt, ...) do { return (result); }while(0)

#endif

}
}
}
#pragma once

#include "nn/types.h"
#include "nn/Result.h"
#include "nn/util/util_Result.h"
#include "nn/os/ARM/os_ExceptionHandler.h"

#ifdef __cplusplus
// err Macros

enum nnerrFatalErrType{
    NN_ERR_FATAL_TYPE_SYSTEM_COMMON = 0,
    NN_ERR_FATAL_TYPE_NAND_CORRUPTION = 1,
    NN_ERR_FATAL_TYPE_CARD_EJECTION = 2,
    NN_ERR_FATAL_TYPE_EXCEPTION = 3,
    NN_ERR_FATAL_TYPE_RESULT_FAILURE = 4,
    NN_ERR_FATAL_TYPE_LOG_ONLY = 5,
};

#endif

namespace nn{
namespace err{
namespace CTR{
namespace{
    const char PORT_NAME_ERR_F[] = "err:f";
}
    struct Exception{
        u8 mInfo[24];
        nn::os::ARM::ExceptionContext mExContext;
    };

    struct Failure{
        char mMessage[96];
    };

    union Data{
        nn::err::CTR::Exception mException;
        nn::err::CTR::Failure mFailure;
    };

    struct FatalErrInfo{
        bit8 mType;
        u8 mRevisionHi;
        ushort mRevisionLo;
        nnResult mResult;
        uptr mPc;
        bit32 mProcessId;
        bit64 mTitleId;
        bit64 mAppTitleId;
        nn::err::CTR::Data mData;
    };

    class FatalErr{
    public:
        Handle mSession;
        
        FatalErr(Handle h){ this->mSession = h; }
        Result Throw(err::CTR::FatalErrInfo& info);
    };
#if NN_VERSION_MAJOR > 2
    void ThrowFatalErr(Result result, uptr pc);
    void ThrowFatalErr(Result result, nnerrFatalErrType type, uptr pc);
    void ThrowFatalErrAll(Result, uptr pc);
#else
    void ThrowFatalErr(Result result, nnerrFatalErrType type, uint pc);
    void ThrowFatalErr(Result result, nnerrFatalErrType type);
    void ThrowFatalErr(Result res);
    void ThrowFatalErrAll(Result res);
#endif
namespace detail{
    template <bool(*IsTarget)(Result), void(*TargetFunc)(Result, uptr)>
    inline void CallIf(Result r, uptr pc){
        if (IsTarget(r)){
            TargetFunc(r, pc);
        }
    }

    template <bool(*IsTarget)(Result), void(*TargetFunc)(Result, const char*, int, uptr)>
    inline void CallIf(Result r, const char* fileName, int lineno, uptr pc){
        if (IsTarget(r)){
            TargetFunc(r, fileName, lineno, pc);
        }
    }

    inline bool IsResultFailure(Result r) {return r.IsFailure(); }
    inline bool IsResultFatal  (Result r) {return r.GetLevel() == ::Result::LEVEL_FATAL; }

}
} 
}
}


#ifndef NN_HARDWARE_CTR_LEGACY
    #ifdef NN_PROCESSOR_ARM11MPCORE
        #if NN_VERSION_MAJOR > 2

            #define NN_ERR_CTR_ERR_API_H_CALL_IF(result, test, f) \
                ::nn::err::CTR::detail::CallIf \
                    < ::nn::err::CTR::detail::test, \
                      ::nn::err::CTR::f >(result, __current_pc())

            #define NN_ERR_CTR_ERR_API_H_CALL_IF2(result, test, f) \
                ::nn::err::CTR::detail::CallIf \
                    < ::nn::err::CTR::detail::test, \
                      ::nn::err::CTR::f >(result, NN_FILE_NAME, __LINE__, __current_pc())

            #define NN_ERR_THROW_FATAL_IF_FATAL_ONLY(result) \
                NN_ERR_CTR_ERR_API_H_CALL_IF(result, IsResultFatal, ThrowFatalErrAll)

            #define NN_ERR_THROW_FATAL(result) \
                NN_ERR_CTR_ERR_API_H_CALL_IF(result, IsResultFailure, ThrowFatalErr)

            #define NN_ERR_THROW_FATAL_ALL(result) \
                NN_ERR_CTR_ERR_API_H_CALL_IF(result, IsResultFailure, ThrowFatalErrAll)

            #ifndef NN_SWITCH_DISABLE_DEBUG_PRINT

                #define NN_ERR_LOG_AND_PANIC_IF_FAILED(result) \
                    NN_ERR_CTR_ERR_API_H_CALL_IF2(result, IsResultFailure, LogAndPanic)

            #else

                #define NN_ERR_LOG_AND_PANIC_IF_FAILED(result) \
                    NN_ERR_CTR_ERR_API_H_CALL_IF(result, IsResultFailure, LogAndPanic)

            #endif

        #else

            #define NN_ERR_THROW_FATAL_IF_FATAL_ONLY(result) \
                do { \
                    ::nn::Result resultLocal = (result); \
                    if (resultLocal.GetLevel() == ::nn::Result::LEVEL_FATAL) { \
                        ::nn::err::CTR::ThrowFatalErrAll(resultLocal); \
                    } \
                } while (0)

            #define NN_ERR_THROW_FATAL(result) \
                do \
                { \
                    ::nn::Result resultLocal = (result); \
                    if ( resultLocal.IsFailure() ) \
                    { \
                        ::nn::err::ThrowFatalErr(resultLocal); \
                    } \
                } while(0)


            #define NN_ERR_THROW_FATAL_ALL(result) \
                do { \
                    ::nn::Result resultLocal = (result); \
                    if (resultLocal.IsFailure()) { \
                        ::nn::err::CTR::ThrowFatalErrAll(resultLocal); \
                    } \
                } while (0)

            #define NN_ERR_LOG_AND_PANIC_IF_FAILED(result) \
                NN_ERR_THROW_FATAL_ALL(result)

        #endif // NN_VERSION_MAJOR > 2

    #else // !NN_PROCESSOR_ARM11MPCORE

        #define NN_ERR_THROW_FATAL_IF_FATAL_ONLY(result) \
            NN_UTIL_PANIC_IF_FAILED(result)

        #define NN_ERR_THROW_FATAL(result) \
            NN_UTIL_PANIC_IF_FAILED(result)

        #define NN_ERR_THROW_FATAL_ALL(result) \
            NN_UTIL_PANIC_IF_FAILED(result)

        #define NN_ERR_LOG_AND_PANIC_IF_FAILED(result) \
            NN_UTIL_PANIC_IF_FAILED(result)

    #endif // NN_PROCESSOR_ARM11MPCORE

#else // NN_HARDWARE_CTR_LEGACY

    #define NN_ERR_THROW_FATAL_IF_FATAL_ONLY(result) \
        ((void)(result))

    #define NN_ERR_THROW_FATAL(result) \
        ((void)(result))

    #define NN_ERR_THROW_FATAL_ALL(result) \
        ((void)(result))

    #define NN_ERR_LOG_AND_PANIC_IF_FAILED(result) \
        ((void)(result))

#endif // NN_HARDWARE_CTR_LEGACY

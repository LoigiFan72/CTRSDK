// Filename: err_Api.cpp
//
// Project: Horizon

#include <nn/err/CTR/err_Api.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/dbg/dbg_DebugString.h>
#include <nn/os/os_Thread.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/os/os_LightEvent.h>
#include <nn/version.h>

using namespace nn::dbg::detail;

namespace nn{
namespace err{
namespace CTR{
namespace{
    
static FatalErrInfo s_FatalErrInfo;
Handle s_FatalErrSession = INVALID_HANDLE_VALUE;
os::CriticalSection s_Lock = nn::WithInitialize();


Result InitializeFatalErrSession(){
    Result res;
    if(!s_FatalErrSession.IsValid()){
        res = svc::ConnectToPort(&s_FatalErrSession,PORT_NAME_ERR_F);
        if(!res.IsSuccess()){
            return res;
        }
    }
    return ResultSuccess();
}

void FinalizeFatalErrSession(){
    if(s_FatalErrSession.IsValid()){
        svc::CloseHandle(s_FatalErrSession);
        s_FatalErrSession = INVALID_HANDLE_VALUE;
    }
}

}

#if NN_VERSION_MAJOR > 2

namespace{
    NN_NOINLINE void Throw(FatalErrInfo& info){
        for(;;){
            os::CriticalSection::ScopedLock lock(s_Lock);
            Result res = InitializeFatalErrSession();
            if(res.IsSuccess()){
                svc::GetProcessId(&info.processId, PSEUDO_HANDLE_CURRENT_PROCESS);
                FatalErr fe(s_FatalErrSession);
                fe.FatalErr::Throw(info);
                FinalizeFatalErrSession();
            }
            else if (res == Result(0xd0401834)){
                if(info.type == NN_ERR_FATAL_TYPE_CARD_EJECTION){
                    os::Thread::Sleep(fnd::TimeSpan::FromMilliSeconds(5));
                    continue;
                }
            }
            else{
                NN_DBG_PRINT_RESULT(res);
            }
            break;
        }
        if (info.type != NN_ERR_FATAL_TYPE_CARD_EJECTION && info.type != NN_ERR_FATAL_TYPE_LOG_ONLY){
            os::LightEvent infiniteLock(true);
            infiniteLock.Wait();
        } 
    }

}

    void ThrowFatalErr(Result result, nnerrFatalErrType type, uptr pc){
        NN_TLOG_("----\n",);
        NN_TLOG_("Fatal Error occured. (pc=0x%08x)\n", pc);
        NN_DBG_PRINT_RESULT(result);

        {
            FatalErrInfo& fei = s_FatalErrInfo;
            s_FatalErrInfo.revisionHi = 0;
            s_FatalErrInfo.revisionLo = 0xc449;
            s_FatalErrInfo.type = type;
            s_FatalErrInfo.result = result;
            s_FatalErrInfo.pc = pc;
            Throw(s_FatalErrInfo);
        }
    }

    void ThrowFatalErr(Result result, uptr pc){
        Result::Level lev = result.Result::GetLevel();
        if(lev == Result::LEVEL_INFO || lev == Result::LEVEL_STATUS)
            return;
        ThrowFatalErr(result, NN_ERR_FATAL_TYPE_SYSTEM_COMMON, pc);
    }

    void ThrowFatalErrAll(Result result, uptr pc){
        ThrowFatalErr(result, NN_ERR_FATAL_TYPE_SYSTEM_COMMON, pc);
    }

#else
namespace{
    void Throw(FatalErrInfo& info){
        os::CriticalSection::ScopedLock lock(s_Lock);
        Result res = InitializeFatalErrSession();
        if(res.IsSuccess()){
            svc::GetProcessId(&info.processId, PSEUDO_HANDLE_CURRENT_PROCESS);
            FatalErr fe(s_FatalErrSession);
            fe.FatalErr::Throw(info);
            FinalizeFatalErrSession();
        }
        else{
            NN_DBG_PRINT_RESULT(res);
        }
        if (info.type != NN_ERR_FATAL_TYPE_CARD_EJECTION){
            os::LightEvent infiniteLock(true);
            infiniteLock.Wait();
        }
    }
}
    void ThrowFatalErr(Result result, nnerrFatalErrType type, uint pc){
        {
            FatalErrInfo& fei = s_FatalErrInfo;
            s_FatalErrInfo.revisionHi = 0;
            s_FatalErrInfo.revisionLo = 0xa037;
            s_FatalErrInfo.type = type;
            s_FatalErrInfo.result = result;
            s_FatalErrInfo.pc = pc;
            Throw(s_FatalErrInfo);
        }
    }

    void ThrowFatalErr(Result result, nnerrFatalErrType type){
        uint bits;
        return ThrowFatalErr(result, type, bits);
    }

    void ThrowFatalErr(Result result){
        Result::Level lev = result.Result::GetLevel();
        if(lev == Result::LEVEL_INFO || lev == Result::LEVEL_STATUS)
            return;
        ThrowFatalErr(result, NN_ERR_FATAL_TYPE_SYSTEM_COMMON);
    }

    void ThrowFatalErrAll(Result result, nnerrFatalErrType type){
        return ThrowFatalErr(result, type, __return_address());
    }
#endif

}
}
}
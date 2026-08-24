#pragma once

#include <nn/os/os_Synchronization.h>
#include <nn/util/util_Result.h>
#include <nn/os/os_Result.h>

namespace nn{
namespace os{

class Mutex : public WaitObject{
public:
    Mutex::~Mutex() { this->Close(); }
    Result TryInitializeImpl(bool initialLocked){
        Handle handle;
        NN_UTIL_RETURN_IF_FAILED(nn::svc::CreateMutex(&handle, initialLocked));
        this->SetHandle(handle);
        return ResultSuccess();
    }
    Result TryInitialize(bool initialLocked){
        Result result = TryInitializeImpl(initialLocked);
        if (result.GetSummary() == Result::SUMMARY_OUT_OF_RESOURCE){
            return result;
        }
        NN_OS_ERROR_IF_FAILED(result);
        return result;
    }
    void Initialize(bool initialLocked){
        NN_OS_ERROR_IF_FAILED(TryInitializeImpl(initialLocked));
    }
    void Finalize() { this->WaitObject::Finalize(); }
    void Lock() { this->WaitObject::WaitOne(); }
    bool TryLock(nn::fnd::TimeSpan timeout = 0) { return this->WaitOne(timeout); }
    void Unlock(){
        NN_OS_ERROR_IF_FAILED(nn::svc::ReleaseMutex(GetHandle()));
    }
};

}
}
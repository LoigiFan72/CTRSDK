#pragma once

#include <nn/Result.h>

namespace nn { 
namespace srv {

enum ResultDescription{
    DESCRIPTION_TOO_LONG_SERVICE_NAME = 5,
};

inline Result MakePermanentResult(Result::Summary summary, int description){
    return MakePermanentResult(summary, Result::MODULE_NN_SRV, description); 
}

inline Result MakeInfoResult(Result::Summary summary, int description){
    return nn::MakeInfoResult(summary, Result::MODULE_NN_SRV, description);
}

inline Result ResultAlreadyInitialized(){
    return MakeInfoResult(Result::SUMMARY_INVALID_STATE, Result::DESCRIPTION_ALREADY_INITIALIZED);
}

inline Result ResultTooLongServiceName(){
    return MakePermanentResult(Result::SUMMARY_WRONG_ARGUMENT, DESCRIPTION_TOO_LONG_SERVICE_NAME);
}

inline Result ResultNotInitialized(){
    return MakePermanentResult(Result::SUMMARY_INVALID_STATE, Result::DESCRIPTION_NOT_INITIALIZED);
}

}
}
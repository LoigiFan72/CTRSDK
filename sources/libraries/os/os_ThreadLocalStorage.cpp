// Filename: os_ThreadLocal.cpp
//
// Project: Horizon

#include <nn/os/os_ThreadLocalStorage.h>
#include <nn/os/CTR/os_ThreadLocalRegion.h>
#include <nn/os/os_Thread.h>
#include <nn/os/CTR/os_CppException.h>

namespace nn{
namespace os{
namespace{
    static int TLS_NUM = 16;

    inline bool IsMappedIndex(s32 index){
        if((index < 0) || (TLS_NUM <= index) ){
            return false;
        }

        return ((sTLSMap >> index) & 1) == 1;
    }

    s32 FreeTLSIndex(int index){
        if(!IsMappedIndex(index)){
            return -1;
        }
        return sTLSMap &= ~(1 << index);
    }
}

ThreadLocalStorage::~ThreadLocalStorage(){
    if(IsMappedIndex(this->mIndex)){
        this->Finalize();
    }
}

void ThreadLocalStorage::Finalize(){
    NN_TASSERT_(this->mIndex >= 0 && this->mIndex < TLS_NUM);
    NN_TASSERT_(IsMappedIndex(this->mIndex));

    FreeTLSIndex(this->mIndex);
}

void ThreadLocalStorage::ClearAllSlots(){
    CTR::ThreadLocalRegion& tlr = *CTR::GetThreadLocalRegion();
    for(int i = 0; i < TLS_NUM; ++i){
        tlr.tls[i] = 0;
    }
}

}
}
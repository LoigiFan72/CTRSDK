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
    ushort s_TLSMap;
}
    
inline CTR::ThreadLocalRegion* GetThreadLocalBuffer(){
    return CTR::GetThreadLocalRegion();
}

namespace{
    static int TLS_NUM = 16;

    inline bool IsMappedIndex(s32 index){
        if((index < 0) || (TLS_NUM <= index) ){
            return false;
        }

        return ((s_TLSMap >> index) & 1) == 1;
    }

    s32 FreeTLSIndex(int index){
        if(!IsMappedIndex(index)){
            return -1;
        }
        return s_TLSMap &= ~(1 << index);
    }
}

ThreadLocalStorage::~ThreadLocalStorage(){
    if(IsMappedIndex(m_Index)){
        this->Finalize();
    }
}

void ThreadLocalStorage::Finalize(){
    NN_TASSERT_(m_Index >= 0 && m_Index < TLS_NUM);
    NN_TASSERT_(IsMappedIndex(this->m_Index));

    FreeTLSIndex(this->m_Index);
}

void ThreadLocalStorage::ClearAllSlots(){
    CTR::ThreadLocalRegion& tlr = *CTR::GetThreadLocalRegion();
    for(int i = 0; i < TLS_NUM; ++i){
        tlr.tls[i] = 0;
    }
}

uptr ThreadLocalStorage::GetValue() const{
    NN_TASSERT_(m_Index >= 0 && m_Index < TLS_NUM);
    NN_TASSERT_(IsMappedIndex(m_Index));
    if (IsMappedIndex(this->m_Index)){
        return GetThreadLocalBuffer()->tls[m_Index];
    }
    else{
        return 0;
    }
}

void ThreadLocalStorage::SetValue(uptr value){
    NN_TASSERT_(m_Index >= 0 && m_Index < TLS_NUM);
    NN_TASSERT_(IsMappedIndex(m_Index));
    if (IsMappedIndex(m_Index)){
        GetThreadLocalBuffer()->tls[m_Index] = value;
    }
}

}
}
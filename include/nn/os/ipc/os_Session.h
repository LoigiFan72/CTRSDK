#pragma once

#include <nn/os/os_Synchronization.h>

#ifdef __cplusplus

namespace nn {
namespace os {
namespace ipc {

class Session : public WaitObject{
public:
    Session(){};
    ~Session(){ this->Finalize(); }
    void Close(){ this->WaitObject::Close(); }
};

}
}
}

#endif
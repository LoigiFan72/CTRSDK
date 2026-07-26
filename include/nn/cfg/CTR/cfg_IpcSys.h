#pragma once

#include <nn/Handle.h>

namespace nn{
namespace cfg{
namespace CTR{
namespace detail{

class IpcSys{
public:
    static nn::Handle sSession;
};

}
}
}
}
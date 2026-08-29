#pragma once

#include <nn/Handle.h>

namespace nn{
namespace cfg{
namespace CTR{
namespace detail{

class IpcInit{
public:
    static nn::Handle s_Session;
};

}
}
}
}
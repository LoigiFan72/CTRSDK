#pragma once

#include <nn/Handle.h>

namespace nn{
namespace srv{
namespace detail{

class Manager{
public:
    static Handle s_Session;
};

}
}
}
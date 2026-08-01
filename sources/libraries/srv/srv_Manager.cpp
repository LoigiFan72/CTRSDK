// Filename: srv_Manager.cpp
//
// Project: Horizon

#include <nn/srv/srv_Manager.h>

namespace nn{
namespace srv{
namespace detail{

Handle Manager::sSession = nn::WithoutInitialize();

}
}
}
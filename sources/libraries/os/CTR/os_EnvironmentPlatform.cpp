// Filename: os_EnvironmentPlatform.cpp
//
// Project: Horizon

#include <nn/os/CTR/os_Environment.h>
#include <nn/applet/CTR/applet_Connect.h>
#include <nn/assert.h>
#include <nn/module.h>

namespace nn { 
namespace os { 
namespace CTR {
namespace 
{
    NN_MAKE_MODULE(s_UseIsRunOnSnake, "NINTENDO", "IsRunOnSnake");

bool IsRunOnSnakeImpl()
{
    NN_REFER_MODULE(s_UseIsRunOnSnake);
    nn::ptm::CTR::TargetPlatform platform;
    NN_TPANIC_(nn::applet::CTR::detail::GetTargetPlatform(&platform));
    return platform != nn::ptm::CTR::TARGET_PLATFORM_CTR;
}

}

bool IsRunOnSnake()
{
    static bool isRunOnSnake = IsRunOnSnakeImpl();
    return isRunOnSnake;
}

}
}
}
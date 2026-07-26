#pragma once

#include "nn/types.h"

#pragma push
#pragma Otime

namespace nn{
namespace math{

class MTX23{
public:
    union{
        float matrix[2][3];
    };
public:
};

}
}
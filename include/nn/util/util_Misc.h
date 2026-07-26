#pragma once

#include <nn/types.h>

namespace nn { 
namespace util {

template <typename T, size_t Num>
char (*NumOfElementsT(T (&a)[Num]))[Num];

}
}
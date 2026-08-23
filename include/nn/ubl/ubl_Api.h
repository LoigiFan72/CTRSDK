#pragma once

#include <nn/fnd.h>
#include <nn/Result.h>

namespace nn{
namespace ubl{

Result Initialize();
void Finalize();
bool IsExist(u64 authorId , u32 titleId , u64 dataId);
u64 GetUserId();

}
}
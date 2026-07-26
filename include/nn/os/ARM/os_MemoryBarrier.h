#pragma once

namespace nn{
namespace os{
namespace ARM{

inline void DataSynchronizationBarrier(){
    __asm("mcr p15, 0, 0, c7, c10, 4");
}

inline void DataMemoryBarrier(){
    __asm("mcr p15, 0, 0, c7, c10, 5");
}

}
}
}
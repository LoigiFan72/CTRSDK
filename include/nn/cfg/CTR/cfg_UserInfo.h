#pragma once

#include <nn/types.h>

namespace nn {
namespace cfg {
namespace CTR {
struct UserName{
        wchar_t userName[11];
        bool    isNgUserName;
};
        
struct Birthday{
        s8 month;
        s8 day;
};

struct SimpleAddress{
        u32 id;
        wchar_t countryName[64];
        wchar_t regionName[16];
        u16 latitude;
        u16 longitude;
};

}
}
}
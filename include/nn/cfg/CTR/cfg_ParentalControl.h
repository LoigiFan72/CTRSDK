#pragma once

#include <nn/types.h>

namespace nn {
namespace cfg {
namespace CTR {
        struct ParentalControlFlags{
                u32 isSetParentalControl;
                u32 browser;
                u32 ulcd;
                u32 photoExchange;
                u32 p2pInternet;
                u32 p2pCec;
                u32 addFriend;
                u32 dsDownload;
                u32 shopPoint;
                u32 video;
                u32 rsv;
        };

        struct ParentalControlInfo{
                nn::cfg::CTR::ParentalControlFlags flags;
                u32  rsv1;
                u8   ogn;
                u8   ratingAge;
                u8   secretQuestionID;
                u8   secretAnswerLength;
                char password[5];
                u8   rsv2[3];
                wchar_t secretAnswer[65];
                u8 rsv3[42];
        };

} // namespace CTR
} // namespace cfg
} // namespace nn
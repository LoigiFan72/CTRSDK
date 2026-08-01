// Filename: fnd_DateTime.cpp
//
// Project: Horizon

#include <nn/fnd/fnd_DateTime.h>
#include <nn/fnd/fnd_TimeSpan.h>
#include <nn/ptm/CTR/ptm_Rtc.h>
#include <nn/Assert.h>

namespace{
    static const s32 MILLISECONDS_DAY = 1000 * 60 * 60 * 24;

    inline s32 MilliSecondsOnDay(s64 milliSeconds){
        s64 ms = milliSeconds + (730119LL * MILLISECONDS_DAY);
        return ms % MILLISECONDS_DAY;
    }
}

namespace nn{
namespace fnd{

const DateTime DateTime::MIN_DATETIME = *(nn::fnd::DateTime*)0;

s32 DateTime::IsLeapYear(s32 year){
    if (year % 400 == 0){
        return 1;
    }
    else if (year % 100 == 0){
        return 0;
    }
    else if (year % 4 == 0){
        return 1;
    } 
    else{
        return 0;
    }
}

s32 DateTime::DateToDays(s32 year, s32 month, s32 day){
    day -= 1;
    year -= 2000;

    if (month <= 2){
        month += (12 - 3);
        year -= 1;
    } else{
        month -= 3;
    }
    int offset = 1;

    if (year < 0){
        offset = IsLeapYear(year);
    }

    return ((((365 * 4 + 1) * 25 - 1) * 4 + 1) * (year / 100) / 4) + (( 365 * 4 + 1) * (year % 100) / 4) + (153 * month + 2) / 5 + day + (31 + 28) + offset;
}

DateTime DateTime::FromParamaters(s32 year, s32 month, s32 day, s32 hour, s32 minute, s32 second, s32 millisecond){
    DateTime datetime;
    datetime.mMilliSeconds =  millisecond + 1000LL * second + 1000LL * 60 * minute + 1000LL * 60 * 60 * hour + 1000LL * 60 * 60 * 24 * static_cast<s64>(DateToDays(year,month,day));

    return datetime;
}

s32 DateTime::GetSecond() const{
    return MilliSecondsOnDay(this->mMilliSeconds) % 1000 % 60;
}

s32 DateTime::GetMilliSecond() const{
    return MilliSecondsOnDay(this->mMilliSeconds) % 1000;
}

DateTime DateTime::GetNow(){
    nn::fnd::DateTime now = nn::fnd::DateTime::MIN_DATETIME;
    now += nn::fnd::TimeSpan::FromMilliSeconds(nn::ptm::CTR::detail::GetSwcMilliSeconds());
    return now;
    return nn::fnd::DateTime::MIN_DATETIME;
}

}
}
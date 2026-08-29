#pragma once

#include <nn/fnd/fnd_TimeSpan.h>

namespace nn{
namespace fnd{

enum Week{
    WEEK_SUNDAY = 0,
    WEEK_MONDAY,
    WEEK_TUESDAY,
    WEEK_WEDNESDAY,
    WEEK_THURSDAY,
    WEEK_FRIDAY,
    WEEK_SATURDAY,
    WEEK_MAX
};

struct DateTimeParameters{
    s32     year;
    s8      month;
    s8      day;
    Week    week;
    s8      hour;
    s8      minute;
    s8      second;
    s16     milliSecond;
};

class DateTime{
    s64 mMilliSeconds;

    static const s64 MIN_MILLISECONDS = -3155673600000LL;
    static const s64 MAX_MILLISECONDS = 5995900800000LL - 1;

    DateTime(s64 milliseconds): 
        mMilliSeconds(milliseconds) { 
        NN_ASSERT_(MIN_MILLISECONDS <= milliseconds && milliseconds <= MAX_MILLISECONDS); 
    }
public:
    static const s32 MILLISECONDS_DAY = 1000 * 60 * 60 * 24;

    // Funni macro hack

    DateTime(): mMilliSeconds(0) {}
    DateTime(s32 year, s32 month, s32 day, s32 hour = 0, s32 minute = 0, s32 second = 0, s32 millisecond = 0): mMilliSeconds(DateTime::FromParamaters(year,month,day,hour,minute,second,millisecond).mMilliSeconds){}

    DateTime FromParamaters(s32,s32,s32,s32,s32,s32,s32);
    s32 GetSecond() const;
    s32 GetMilliSecond() const;
    static DateTime GetNow();
    s32 DateToDays(s32,s32,s32);

    DateTime& operator+=(const TimeSpan& rhs) { this->mMilliSeconds += rhs.GetMilliSeconds(); return *this; }
    friend DateTime operator+(const DateTime& lhs, const TimeSpan& rhs) { DateTime ret(lhs); return ret += rhs; }

    DateTime& operator-=(const TimeSpan& rhs) { this->mMilliSeconds -= rhs.GetMilliSeconds(); return *this; }
    friend DateTime operator-(const DateTime& lhs, const TimeSpan& rhs) { DateTime ret(lhs); return ret -= rhs; }
    friend TimeSpan operator-(const DateTime& lhs, const DateTime& rhs) { return TimeSpan::FromMilliSeconds(lhs.mMilliSeconds - rhs.mMilliSeconds); }

    static s32  IsLeapYear(s32 year);

    static const DateTime MIN_DATETIME;
};
}
}
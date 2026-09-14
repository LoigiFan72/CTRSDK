#pragma once

#include <nn/fnd/fnd_TimeSpan.h>

namespace nn{
namespace fnd{

enum Week
{
    WEEK_SUNDAY = 0,
    WEEK_MONDAY,
    WEEK_TUESDAY,
    WEEK_WEDNESDAY,
    WEEK_THURSDAY,
    WEEK_FRIDAY,
    WEEK_SATURDAY,
    WEEK_MAX
};

struct DateTimeParameters
{
    s32     year;
    s8      month;
    s8      day;
    Week    week;
    s8      hour;
    s8      minute;
    s8      second;
    s16     milliSecond;
};

class DateTime
{
    s64 m_MilliSeconds;

    static const s64 MIN_MILLISECONDS = -3155673600000LL;
    static const s64 MAX_MILLISECONDS = 5995900800000LL - 1;

    DateTime(s64 milliseconds): 
        m_MilliSeconds(milliseconds) 
    { 
        NN_ASSERT_(MIN_MILLISECONDS <= milliseconds && milliseconds <= MAX_MILLISECONDS); 
    }
public:
    static const s32 MILLISECONDS_DAY = 1000 * 60 * 60 * 24;

    DateTime(): 
        m_MilliSeconds(0) 
    {
    }
    DateTime(s32 year, s32 month, s32 day, s32 hour = 0, s32 minute = 0, s32 second = 0, s32 millisecond = 0): 
        m_MilliSeconds(DateTime::FromParamaters(year,month,day,hour,minute,second,millisecond).m_MilliSeconds)
    {
    }

    DateTime FromParamaters(s32,s32,s32,s32,s32,s32,s32);

    s32 GetYear() const;
    s32 GetMonth() const;
    s32 GetDay() const;
    Week GetWeek() const;
    s32 GetHour() const;
    s32 GetMinute() const;
    s32 GetSecond() const;
    s32 GetMilliSecond() const;
    static DateTime GetNow();

    static void DaysToDate(s32 *pYear, s32 *pMonth, s32 *pDay, s32 days);
    static s32  DateToDays(s32 year, s32 month, s32 day);
    static Week DaysToWeekday(s32 days);

    DateTime& operator+=(const TimeSpan& rhs) { m_MilliSeconds += rhs.GetMilliSeconds(); return *this; }
    friend DateTime operator+(const DateTime& lhs, const TimeSpan& rhs) { DateTime ret(lhs); return ret += rhs; }

    DateTime& operator-=(const TimeSpan& rhs) { m_MilliSeconds -= rhs.GetMilliSeconds(); return *this; }
    friend DateTime operator-(const DateTime& lhs, const TimeSpan& rhs) { DateTime ret(lhs); return ret -= rhs; }
    friend TimeSpan operator-(const DateTime& lhs, const DateTime& rhs) { return TimeSpan::FromMilliSeconds(lhs.m_MilliSeconds - rhs.m_MilliSeconds); }

    static s32  IsLeapYear(s32 year);

    static const DateTime MIN_DATETIME;
};
}
}
// Filename: fnd_DateTime.cpp
//
// Project: Horizon

#include <nn/fnd/fnd_DateTime.h>
#include <nn/fnd/fnd_TimeSpan.h>
#include <nn/ptm/CTR/ptm_Rtc.h>
#include <nn/Assert.h>

namespace{
    static const s32 MILLISECONDS_DAY = 1000 * 60 * 60 * 24;

    inline s32 Modulo32(s32 a, s32 b)
    {
        return (((a % b) + b) % b);
    }

    inline s32 MilliSecondsOnDay(s64 milliSeconds)
    {
        s64 ms = milliSeconds + (730119LL * MILLISECONDS_DAY);
        return ms % MILLISECONDS_DAY;
    }

    inline s32 AlignedDays(s64 milliSeconds)
    {
        s64 ms = milliSeconds + ( 730119LL * MILLISECONDS_DAY);
        return ms / MILLISECONDS_DAY - 730119LL;
    }
}

namespace nn{
namespace fnd{

const DateTime DateTime::MIN_DATETIME = *(nn::fnd::DateTime*)0;

s32 DateTime::IsLeapYear(s32 year)
{
    if (year % 400 == 0)
    {
        return 1;
    }
    else if (year % 100 == 0)
    {
        return 0;
    }
    else if (year % 4 == 0)
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

s32 DateTime::DateToDays(s32 year, s32 month, s32 day)
{
    day -= 1;
    year -= 2000;

    if (month <= 2)
    {
        month += (12 - 3);
        year -= 1;
    } 
    else
    {
        month -= 3;
    }
    int offset = 1;

    if (year < 0)
    {
        offset = IsLeapYear(year);
    }

    return ((((365 * 4 + 1) * 25 - 1) * 4 + 1) * (year / 100) / 4) + (( 365 * 4 + 1) * (year % 100) / 4) + (153 * month + 2) / 5 + day + (31 + 28) + offset;
}

void DateTime::DaysToDate(s32 *pYear, s32 *pMonth, s32 *pDay, s32 days)
{
    const s32 c4days = (((365 * 4 + 1) * 25 - 1) * 4 + 1);
    const s32 c1days =  ((365 * 4 + 1) * 25 - 1);
    const s32 y4days =   (365 * 4 + 1);
    const s32 y1days =    365;
    s32 year, month, day;

    days -= 31 + 29;

    s32 c4    = days / c4days;
    s32 c4ds  = days % c4days;

    if (c4ds < 0)
    {
        c4ds += c4days;
        c4 -= 1;
    }

    s32 c1    = c4ds / c1days;
    s32 c1ds  = c4ds % c1days;
    s32 y4    = c1ds / y4days;
    s32 y4ds  = c1ds % y4days;
    s32 y1    = y4ds / y1days;
    s32 ydays = y4ds % y1days;

    year = 2000 + c4 * 400 + c1 * 100 + y4 * 4 + y1;
    month = (5 * ydays + 2) / 153;
    day = ydays - (153 * month + 2) / 5 + 1;

    if (y1 == 4 || c1 == 4)
    {
        month = 2 + (12 - 3);
        day = 29;
        year -= 1;
    }

    if (month <= (12-3))
    {
        month += 3;
    }
    else
    {
        month -= 12 - 3;
        year += 1;
    }

    if (pYear)
    {
        *pYear = year;
    }
    if (pMonth)
    {
        *pMonth = month;
    }
    if (pDay)
    {
        *pDay = day;
    }
}

s32 DateTime::DateToDays(s32 year, s32 month, s32 day)
{
    NN_TASSERT_(DateTime::IsValidDate(year,month,day));

    day -= 1;
    year -= 2000;

    if (month <= 2)
    {
        month += (12 - 3);
        year -= 1;
    }
    else
    {
        month -= 3;
    }

    int offset = 1;

    if (year < 0)
    {
        offset = IsLeapYear(year);
    }

    return ((((365 * 4 + 1) * 25 - 1) * 4 + 1) * (year / 100) / 4) + (( 365 * 4 + 1) * (year % 100) / 4)
            + (153 * month + 2) / 5 + day + (31 + 28) + offset;
}

Week DateTime::DaysToWeekday(s32 days)
{
    return static_cast<Week>(Modulo32((days + WEEK_SATURDAY), WEEK_MAX));
}

DateTime DateTime::FromParamaters(s32 year, s32 month, s32 day, s32 hour, s32 minute, s32 second, s32 millisecond)
{
    DateTime datetime;
    datetime.m_MilliSeconds =  millisecond + 1000LL * second + 1000LL * 60 * minute + 1000LL * 60 * 60 * hour + 1000LL * 60 * 60 * 24 * static_cast<s64>(DateToDays(year,month,day));

    return datetime;
}

s32 DateTime::GetYear() const
{
    s32 year;
    DaysToDate(&year, NULL, NULL, AlignedDays(m_MilliSeconds));
    return year;
}

s32 DateTime::GetMonth() const
{
    s32 month;
    DaysToDate(NULL, &month, NULL, AlignedDays(m_MilliSeconds));
    return month;
}

s32 DateTime::GetDay() const
{
    s32 day;
    DaysToDate(NULL, NULL, &day, AlignedDays(m_MilliSeconds));
    return day;
}

Week DateTime::GetWeek() const
{
    return DaysToWeekday(AlignedDays(m_MilliSeconds));
}

s32 DateTime::GetHour() const        
{
    return MilliSecondsOnDay(m_MilliSeconds) / 1000 / 60 / 60 % 24; 
}
    
s32 DateTime::GetMinute() const      
{
    return MilliSecondsOnDay(m_MilliSeconds) / 1000 / 60 % 60; 
}

s32 DateTime::GetSecond() const
{
    return MilliSecondsOnDay(m_MilliSeconds) % 1000 % 60;
}

s32 DateTime::GetMilliSecond() const
{
    return MilliSecondsOnDay(m_MilliSeconds) % 1000;
}

DateTime DateTime::GetNow()
{
    nn::fnd::DateTime now = nn::fnd::DateTime::MIN_DATETIME;
    now += nn::fnd::TimeSpan::FromMilliSeconds(nn::ptm::CTR::detail::GetSwcMilliSeconds());
    return now;
    return nn::fnd::DateTime::MIN_DATETIME;
}

}
}
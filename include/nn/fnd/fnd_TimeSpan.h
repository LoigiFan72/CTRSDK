#pragma once

#include <nn/types.h>
#include <nn/Assert.h>

namespace nn{
namespace fnd{

class TimeSpan{
    typedef const class ZeroOnlyTag {} * ZeroOnly;
public:
    TimeSpan(ZeroOnly zeroOnly = 0): 
        m_NanoSeconds(0) 
    {
    }

    s64 GetDays() const { return m_NanoSeconds / (1000LL * 1000 * 1000 * 60 * 60 * 24); }
    s64 GetHours() const { return m_NanoSeconds / (1000LL * 1000 * 1000 * 60 * 60); }
    s64 GetMinutes() const { return m_NanoSeconds / (1000LL * 1000 * 1000 * 60); }
    s64 GetSeconds() const { return m_NanoSeconds / (1000 * 1000 * 1000); }
    s64 GetMilliSeconds() const { return m_NanoSeconds / (1000 * 1000); }
    s64 GetMicroSeconds() const { return m_NanoSeconds / 1000; }
    
    static s64 MultiplyRightShift(const s64 x, const s64 y)
    {
        const u64 x_lo = x & 0xffffffff;
        const s64 x_hi = x >> 32;

        const u64 y_lo = y & 0xffffffff;
        const s64 y_hi = y >> 32;

        const s64 z = x_hi * y_lo + ((x_lo * y_lo) >> 32);
        const s64 z_lo = z & 0xffffffff;
        const s64 z_hi = z >> 32;

        return x_hi * y_hi + z_hi + (static_cast<s64>(x_lo * y_hi + z_lo) >> 32);
    }

    s64 GetNanoSeconds() const
    {
        return m_NanoSeconds;
    }

    s64 DivideNanoSeconds(s64 magic, s32 rightShift) const
    {
        s64 n = MultiplyRightShift(m_NanoSeconds, magic);
        if (magic < 0)
        {
            n += m_NanoSeconds;
        }
        n >>= rightShift;
        return n + (static_cast<u64>(m_NanoSeconds) >> 63);
    }

    s64 GetMilliSeconds() const
    {
        return DivideNanoSeconds(0x431bde82d7b634dbLL, 18); 
    }

    /* Macros Needed elsewhere, and makes it easier for DateTime. */

    static TimeSpan FromNanoSeconds(s64 nanoSeconds) { TimeSpan ret; ret.m_NanoSeconds = nanoSeconds; return ret; }
    static TimeSpan FromMicroSeconds(s64 microSeconds) { return FromNanoSeconds(microSeconds * 1000); }
    static TimeSpan FromMilliSeconds(s64 milliSeconds) { return FromNanoSeconds(milliSeconds * 1000 * 1000); }
    friend bool operator==(const TimeSpan& lhs, const TimeSpan& rhs) { return lhs.m_NanoSeconds == rhs.m_NanoSeconds; }
    friend bool operator< (const TimeSpan& lhs, const TimeSpan& rhs) { return lhs.m_NanoSeconds <  rhs.m_NanoSeconds; }
    TimeSpan& operator-=(const TimeSpan& rhs) { this->m_NanoSeconds -= rhs.m_NanoSeconds; return *this; }
    friend TimeSpan operator-(const TimeSpan& lhs, const TimeSpan& rhs) { TimeSpan ret(lhs); return ret -= rhs; }
private:
    s64 m_NanoSeconds;
};

}
}
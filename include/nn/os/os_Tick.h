#pragma once

#include <nn/fnd/fnd_TimeSpan.h>
#include <nn/math/math_Misc.h>

#define NN_CTR_MPCORE_TICKS_PER_SECOND 134055928LL

namespace nn{
namespace svc{
    s64 GetSystemTick();
}

namespace os{

class Tick{
public:
    s64 mTick;

    static const s64 TICKS_PER_SECOND  = NN_CTR_MPCORE_TICKS_PER_SECOND;

    Tick(s64 tick = 0) : mTick(tick) {}
    Tick(nn::fnd::TimeSpan span);
    static Tick GetSystemCurrent();

    operator s64() const { return mTick; }
    operator nn::fnd::TimeSpan() const;
    Tick& operator-=(Tick rhs);
    Tick operator-(Tick rhs) const;
    nn::fnd::TimeSpan ToTimeSpan() const;
};

inline Tick Tick::GetSystemCurrent(){
    return Tick(nn::svc::GetSystemTick());
}

inline Tick::Tick(nn::fnd::TimeSpan span): 
    mTick(nnmathMultiplyRate32(span.GetNanoSeconds(), math::MakeRate32<TICKS_PER_SECOND, 1000 * 1000 * 1000>::VALUE) )
{}

inline Tick::operator nn::fnd::TimeSpan() const{
    return nn::fnd::TimeSpan::FromNanoSeconds(nnmathMultiplyRate(this->mTick,math::MakeRate<1000 * 1000 * 1000, TICKS_PER_SECOND>::VALUE ));
}

inline nn::fnd::TimeSpan Tick::ToTimeSpan() const{
    return *this;
}

inline Tick& Tick::operator-=(Tick rhs) { this->mTick -= rhs.mTick; return *this; }
inline Tick Tick::operator-(Tick rhs)  const { Tick ret(*this); return ret -= rhs; }

}
}
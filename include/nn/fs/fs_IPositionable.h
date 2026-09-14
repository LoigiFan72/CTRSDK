#pragma once

#include <nn/Result.h>
#include <nn/fs/fs_Parameters.h>

namespace nn{
namespace fs{

class IPositionable
{
protected:
    virtual ~IPositionable() {}
public:
    virtual Result TrySeek(s64 position, PositionBase base) = 0;
    virtual void Seek(s64 position, PositionBase base) = 0;
    
    virtual Result TryGetPosition(s64* pOut) const = 0;
    virtual s64 GetPosition() const = 0;
    
    virtual Result TrySetPosition(s64 position) = 0;
    virtual void SetPosition(s64 position) = 0;
    
    virtual Result TryGetSize(s64* pOut) const = 0;
    virtual s64 GetSize() const = 0;
};

} // fs
} // nn
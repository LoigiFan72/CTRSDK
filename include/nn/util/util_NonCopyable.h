#pragma once

namespace nn {
namespace util {

template <typename T>
class NonCopyable {};

namespace ADLFireWall {

template <typename T>
struct NonCopyable{
protected:
    NonCopyable () {}
    ~NonCopyable () {}
private:
    NonCopyable (const NonCopyable&);
    NonCopyable & operator = (const NonCopyable &);
};

}
}
}
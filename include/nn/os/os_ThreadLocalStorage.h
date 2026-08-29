#pragma once

#include <nn/types.h>
#include <nn/Result.h>

namespace nn{
namespace os{

class ThreadLocalStorage{
public:
    int m_Index;

    ~ThreadLocalStorage();
    void Finalize();
    static void ClearAllSlots();
    uptr GetValue() const;
    void SetValue(uptr value);
};

}
}
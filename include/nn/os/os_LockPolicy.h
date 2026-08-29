#pragma once

#include <nn/types.h>
#include <nn/Result.h>

namespace nn {
namespace os {

class CriticalSection;

class LockPolicy {
public:

    class NoLock {
    public:
        class LockObject {
        public:
            void Initialize() {}
            Result TryInitialize() { return Result(); }
            void Finalize() {}
        };

        class ScopedLock {
        public:
            ScopedLock(const LockObject&) {}
            ~ScopedLock() {}
        };
    };

    template <class Locker>
    class Object {
    public:
        class ScopedLock;

        class LockObject {
        private:
            friend class ScopedLock;
            mutable Locker mutex;
        public:
            void Initialize() { mutex.Initialize(); }
            Result TryInitialize() { return mutex.TryInitialize(); }
            void Finalize() { mutex.Finalize(); }
        };

        class ScopedLock : private Locker::ScopedLock {
        public:
            ScopedLock(const LockObject& param_1) : Locker::ScopedLock(param_1.mutex) {}
        };
    };

    template <class Locker>
    struct Global {
    protected:
        static Locker g_Mutex;
    public:
        class LockObject {
        public:
            void Initialize() {}
            Result TryInitialize() { return Result(); }
            void Finalize() {}
        };

        class ScopedLock : private Locker::ScopedLock {
        public:
            ScopedLock(const LockObject&) : Locker::ScopedLock(gMutex) {}
        };
    };
};

} // namespace os
} // namespace nn
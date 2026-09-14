#pragma once

#include "nn/types.h"
#include "nn/util/util_TypeTraits.h"

// I made claude write this. 
//
// I dont understand half of this shit.

namespace nn {
namespace fnd {

class IAllocator
{
public:
    virtual void* Allocate(size_t size, s32 alignment) = 0;
    virtual void  Free(void* p) = 0;
    virtual ~IAllocator() {}
};

template <typename Allocator, typename Tag>
class StdAllocatorAdapterHolder
{
public:
    static void      SetAllocator(Allocator allocator) { sAllocator = allocator; }
    static Allocator GetAllocator()                    { return sAllocator; }
protected:
    static Allocator sAllocator;
};

template <typename Allocator, typename T, typename Tag = void>
class StdAllocatorAdapter : public StdAllocatorAdapterHolder<Allocator, Tag>
{
public:
    typedef size_t    size_type;
    typedef sptr      difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    template <class U> struct rebind { typedef StdAllocatorAdapter<Allocator, U, Tag> other; };

    StdAllocatorAdapter()throw(){}
    StdAllocatorAdapter(const StdAllocatorAdapter&) throw(){}
    template <class U> StdAllocatorAdapter(const StdAllocatorAdapter<Allocator, U, Tag>&) throw()
    {
    }

    ~StdAllocatorAdapter() throw() {}

    pointer       address(reference& x)       const { return &x; }
    const_pointer address(const_reference& x) const { return &x; }

    pointer allocate(size_type n, void* = 0) 
    {
        return static_cast<T*>(StdAllocatorAdapterHolder<Allocator, Tag>::GetAllocator()->Allocate(n * sizeof(T), nn::util::alignment_of<T>::value));
    }

    void deallocate(pointer p, size_type) 
    {
        StdAllocatorAdapterHolder<Allocator, Tag>::GetAllocator()->Free(static_cast<void*>(p));
    }

    size_type max_size() const throw() 
    { 
        return ::std::numeric_limits<size_type>::max() / sizeof(T); 
    }

    void construct(pointer p, const T& val) 
    {
        new (static_cast<void*>(p)) T(val); 
    }

    void destroy(pointer p) 
    {
        p->~T();
    }

};

template <typename Allocator, typename T, typename Tag = void>
class StdUnitAllocatorAdapter 
{
public:
    static void SetAllocator(Allocator allocator) 
    {
        sAllocator = allocator; 
    }
    static Allocator GetAllocator()
    {
        return sAllocator; 
    }

    typedef size_t    size_type;
    typedef sptr      difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    template <class U> struct rebind { typedef StdUnitAllocatorAdapter<Allocator, U, Tag> other; };

    StdUnitAllocatorAdapter() throw() {}
    StdUnitAllocatorAdapter(const StdUnitAllocatorAdapter&) throw() {}
    template <class U> StdUnitAllocatorAdapter(const StdUnitAllocatorAdapter<Allocator, U, Tag>&) throw() {}
    ~StdUnitAllocatorAdapter() throw() {}

    pointer address(reference& x) 
    { 
        return &x; 
    }
    const_pointer address(const_reference& x) const 
    {
        return &x; 
    }

    pointer allocate(size_type n, void* = 0) {
        return static_cast<T*>(sAllocator->Allocate(n * sizeof(T), nn::util::alignment_of<T>::value));
    }

    void deallocate(pointer p, size_type) {
        sAllocator->Free(static_cast<void*>(p));
    }

    size_type max_size() const throw() {
        return ::std::numeric_limits<size_type>::max() / sizeof(T); 
    }

    void construct(pointer p, const T& val) {
        new (static_cast<void*>(p)) T(val); 
    }
    void destroy(pointer p)                 {
        p->~T(); 
    }

protected:
    static Allocator sAllocator;
};

template <typename Allocator, typename Tag>
Allocator StdAllocatorAdapterHolder<Allocator, Tag>::sAllocator;

template <typename Allocator, typename T, typename Tag>
Allocator StdUnitAllocatorAdapter<Allocator, T, Tag>::sAllocator;

}} // namespace nn::fnd
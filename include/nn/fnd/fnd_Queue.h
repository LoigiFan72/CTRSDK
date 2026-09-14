#pragma once

#include <nn/util/util_NonCopyable.h>
#include <nn/Assert.h>

namespace nn { 
namespace fnd {

template <typename T, typename Tag = void>
class IntrusiveQueue : private nn::util::NonCopyable<IntrusiveQueue<T, Tag> >
{
public:
    
    class Item;

    IntrusiveQueue(): 
        m_Head(0), 
        m_Tail(0) 
    {
    }

    bool IsEmpty() const { return m_Head == 0; }
    void Enqueue(T* p);
    T* Dequeue();
    void Clear();
    
private:
    
    Item* m_Head;
    Item* m_Tail;
    
};

template <typename T, typename Tag>
class IntrusiveQueue<T, Tag>::Item : private nn::util::NonCopyable<IntrusiveQueue<T, Tag>::Item>
{
    friend class IntrusiveQueue;
protected:
    Item() : m_NextLink(0) {}
    ~Item() { NN_TASSERT_(!m_NextLink); }
private:
    Item* m_NextLink;
};

template <typename T, typename Tag>
inline void IntrusiveQueue<T, Tag>::Enqueue(T* p)
{
    NN_TASSERT_(p);
    Item* pNode = static_cast<Item*>(p);
    NN_TASSERT_(!pNode->m_NextLink);
    if (IsEmpty())
    {
        this->m_Head = this->m_Tail = pNode;
        pNode->m_NextLink = pNode;
    }
    else
    {
        m_Tail->m_NextLink = p;
        this->m_Tail = p;
    }
}

template <typename T, typename Tag>
inline T* IntrusiveQueue<T, Tag>::Dequeue()
{
    if (IsEmpty())
    {
        return 0;
    }
    else
    {
        Item* ret = m_Head;
        if (m_Head == m_Tail)
        {
            this->m_Head = 0;
        }
        else
        {
            this->m_Head = m_Head->m_NextLink;
        }
        ret->m_NextLink = 0;
        return static_cast<T*>(ret);
    }
}

template <typename T, typename Tag>
inline void IntrusiveQueue<T, Tag>::Clear()
{
    if (m_Head)
    {
        Item* p = m_Head;
        do
        {
            Item* q = p;
            p = p->m_NextLink;
            q->m_NextLink = 0;
        } while (p != m_Tail);
        this->m_Head = 0;
    }
}

}
}
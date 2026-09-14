#pragma once

#include "nn/Assert.h"
#include "nn/fnd/fnd_Result.h"
#include "nn/util/util_NonCopyable.h"

namespace nn{
namespace fnd{

template <typename T, typename Tag = void>
class IntrusiveLinkedList : private nn::util::NonCopyable<IntrusiveLinkedList<T, Tag> >
{
public:

    class Item;

    void Insert(T* position, T* inserted);
    static void InsertBefore(Item* p, Item* q);
    void PushBack(T* p);
    void PushFront(T* p);
    T* GetNext(T* p) const;
    T* GetBack() const;
    T* GetPrevious(T* p) const;
    T* GetFront() const;
    void Erase(T* p);
    static void ClearLinks(Item* p);
    bool IsEmpty() const {
        return !mHead; 
    }
protected:
    Item* mHead;
};


template <typename T, typename Tag>
class IntrusiveLinkedList<T, Tag>::Item : private nn::util::NonCopyable<IntrusiveLinkedList<T, Tag>::Item> 
{
public:
    Item() : 
        m_PreviousLink(0), 
        m_NextLink(0) 
    {
    }

    ~Item() { NN_TASSERT_(!m_PreviousLink && !m_NextLink); }

    Item* m_PreviousLink;
    Item* m_NextLink;
protected:
};

template <typename T, typename Tag>
inline void IntrusiveLinkedList<T, Tag>::Erase(T* p)
{
    NN_ASSERT_WITH_RESULT(p, MakeResultInvalidAddress());
    Item* pNode = static_cast<Item*>(p);
    NN_ASSERT_WITH_RESULT(pNode->m_PreviousLink, MakeResultInvalidNode());
    if (pNode == pNode->m_PreviousLink)
    {
        this->m_Head = 0;
    }
    else
    {
        if (m_Head == pNode)
        {
            this->m_Head = m_Head->m_NextLink;
        }
        pNode->m_NextLink->m_PreviousLink = pNode->m_PreviousLink;
        pNode->m_PreviousLink->m_NextLink = pNode->m_NextLink;
    }
    ClearLinks(pNode);
}

template <typename T, typename Tag>
inline T* IntrusiveLinkedList<T, Tag>::GetNext(T* p) const
{
    NN_ASSERT_WITH_RESULT(p, MakeResultInvalidAddress());
    Item* pNode = static_cast<Item*>(p);
    NN_ASSERT_WITH_RESULT(pNode->m_PreviousLink, MakeResultInvalidNode());
    if (p == this->GetBack())
    {
        return 0;
    }
    return static_cast<T*>(pNode->m_NextLink);
}

template <typename T, typename Tag>
inline void IntrusiveLinkedList<T, Tag>::ClearLinks(Item* p)
{
    p->m_PreviousLink = p->m_NextLink = 0;
}

template <typename T, typename Tag>
inline void IntrusiveLinkedList<T, Tag>::PushBack(T* p)
{
    NN_ASSERT_WITH_RESULT(p, MakeResultInvalidAddress());
    NN_TASSERT_(p);
    Item* pNode = static_cast<Item*>(p);
    NN_ASSERT_WITH_RESULT(!pNode->m_PreviousLink, MakeResultAlreadyListed());
    NN_TASSERT_(!pNode->m_PreviousLink);
    NN_TASSERT_(!pNode->m_NextLink);
    if (IsEmpty())
    {
        p->m_PreviousLink = p->m_NextLink = p;
        this->m_Head = p;
    }
    else
    {
        InsertBefore(mHead, pNode);
    }
}

template <typename T, typename Tag>
inline T* IntrusiveLinkedList<T, Tag>::GetBack() const
{
    if (IsEmpty())
    {
        return 0;
    }
    else
    {
        return static_cast<T*>(m_Head->m_PreviousLink);
    }
}

template <typename T, typename Tag>
inline void IntrusiveLinkedList<T, Tag>::PushFront(T* p)
{
    NN_ASSERT_WITH_RESULT(p, MakeResultInvalidAddress());
    Item* pNode = static_cast<Item*>(p);
    NN_ASSERT_WITH_RESULT(!pNode->m_PreviousLink, MakeResultAlreadyListed());
    if (IsEmpty())
    {
        p->m_PreviousLink = p->m_NextLink = p;
    }
    else
    {
        InsertBefore(m_Head, pNode);
    }
    this->m_Head = p;
}

template <typename T, typename Tag>
inline T* IntrusiveLinkedList<T, Tag>::GetFront() const
{
    return static_cast<T*>(m_Head);
}

template <typename T, typename Tag>
inline T* IntrusiveLinkedList<T, Tag>::GetPrevious(T* p) const
{
    NN_ASSERT_WITH_RESULT(p, MakeResultInvalidAddress());
    Item* pNode = static_cast<Item*>(p);
    NN_ASSERT_WITH_RESULT(pNode->m_PreviousLink, MakeResultInvalidNode());
    if (p == this->GetFront())
    {
        return 0;
    }
    return static_cast<T*>(pNode->m_PreviousLink);
}

template <typename T, typename Tag>
inline void IntrusiveLinkedList<T, Tag>::InsertBefore(Item* p, Item* q)
{
    q->m_NextLink = p;
    p->m_PreviousLink->m_NextLink = q;
    q->m_PreviousLink = p->m_PreviousLink;
    p->m_PreviousLink = q;
}

template <typename T, typename Tag>
inline void IntrusiveLinkedList<T, Tag>::Insert(T* position, T* inserted)
{
    NN_ASSERT_WITH_RESULT(inserted, MakeResultInvalidAddress());
    Item* pNodeInserted = static_cast<Item*>(inserted);
    Item* pNodePosition = static_cast<Item*>(position);
    NN_ASSERT_WITH_RESULT(!pNodeInserted->m_PreviousLink, MakeResultAlreadyListed());
    if (pNodePosition == m_Head)
    {
        PushFront(inserted);
    }
    else if (pNodePosition)
    {
        NN_ASSERT_WITH_RESULT(pNodePosition->m_PreviousLink, MakeResultInvalidNode());
        InsertBefore(pNodePosition, pNodeInserted);
    }
    else
    {
        PushBack(inserted);
    }
}

}
}

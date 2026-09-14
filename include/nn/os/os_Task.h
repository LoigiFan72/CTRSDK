#pragma once

#include <nn/types.h>
#include <nn/fnd/fnd_Queue.h>

namespace nn{
namespace os{
class ITask
{
public:
};

class ITaskInvoker
{
public:
};

class IWaitTaskInvoker : public ITaskInvoker
{
public:
};

class QueueableTask : public nn::fnd::IntrusiveQueue<QueueableTask>::Item, public ITask
{
public:
};

}
}
#pragma once

#include <nn/types.h>
#include <nn/fnd/fnd_Intrusive.h>

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

class QueueableTask : public ITask, public fnd::IntrusiveQueue::Item
{
public:
};

}
}
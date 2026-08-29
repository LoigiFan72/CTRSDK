#pragma once

#include <nn/types.h>

namespace nn {
namespace cec {
namespace CTR {

typedef u8  CECMessageId[CEC_SIZEOF_MESSAGEID];

class MessageId{
public:
    static const size_t SIZE = CEC_SIZEOF_MESSAGEID;
    static const size_t ENCODED_SIZE = 12;

    MessageId();
    explicit MessageId(const u8 msgId[SIZE]);
    explicit MessageId(CECMessageId msgId);

    bool IsEqual(const u8 msgId[SIZE]) const;
    bool IsEmpty() const;
private:
    u8      m_data[SIZE];
    static  char s_buffer[SIZE * 2 + 1];
};

} // namespace CTR
} // namespace cec
} // namespace nn
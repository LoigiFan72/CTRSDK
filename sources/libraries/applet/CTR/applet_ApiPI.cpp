//
//

#include <nn/applet/CTR/applet_Ipc.h>
#include <nn/applet/CTR/applet_Connect.h>
#include <nn/err.h>

namespace {
    const size_t WRAP_SIZE = 16;
}

namespace nn {
namespace applet {
namespace CTR {
namespace detail {

Result Wrap(void* pWrappedBuffer, const void* pData, size_t dataSize, s32 idOffset, size_t idSize)
{
    Result result;
    detail::LockAndConnect();
    result = detail::APPLET::Wrap(reinterpret_cast<bit8*>(pWrappedBuffer), reinterpret_cast<const bit8*>(pData), dataSize + WRAP_SIZE, dataSize, idOffset, idSize);
    detail::DisconnectAndUnlock();
    return result;
}

Result Unwrap(void* pData, const void* pWrapped, size_t wrappedSize, s32 idOffset, size_t idSize)
{
    Result result;
    detail::LockAndConnect();
    result = detail::APPLET::Unwrap(reinterpret_cast<bit8*>(pData),reinterpret_cast<const bit8*>(pWrapped), wrappedSize - WRAP_SIZE, wrappedSize, idOffset, idSize);
    detail::DisconnectAndUnlock();
    return result;
}

}
}
}
}
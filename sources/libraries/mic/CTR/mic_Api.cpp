// Filename: mic_Api.cpp
//
// Project: Horizon

#include <nn/mic/CTR/mic_Api.h>
#include <nn/mic/CTR/mic_IpcClient.h>
#include <nn/mic/CTR/mic_Result.h>
#include <nn/codec/CTR/codec_Types.h>
#include <nn/os.h>
#include <nn/util.h>
#include <nn/version.h>
#include <nn/srv.h>

namespace nn{
namespace mic{
namespace CTR{
namespace detail{
namespace {
    bool isInitialized = false;
}
    
Result InitializeBase(Handle* pSession, const char* name){
    if(isInitialized){
        return ResultAlreadyInitialized();
    }

    Result result = nn::srv::Initialize();
    if (result.GetDescription() != nn::Result::DESCRIPTION_ALREADY_INITIALIZED ){
        NN_UTIL_PANIC_IF_FAILED(result);
    }

    result = nn::srv::GetServiceHandle(pSession, name);

    if(result.IsFailure()){
        if (result == nn::os::ResultOverPortCapacity() ){
            return ResultUsingOtherProcess();
        }
        NN_UTIL_PANIC_IF_FAILED(result);
    }
    isInitialized = true;
    return result;
}

Result FinalizeBase(Handle* pSession){
    if(!isInitialized){
        return ResultNotInitialized();
    }
    StopSampling();
    ResetBuffer();
    isInitialized = false;
    return nn::svc::CloseHandle(*pSession);
}

} // detail

namespace{
    const char PORT_NAME_USER[] = "mic:u";
    nn::os::TransferMemoryBlock sSharedMemory;
    bool sIsBufferInitialized = false;
    bool sIsLpfEnable = false;
    SamplingRate sSamplingRate;
    size_t user_size;

	const nn::codec::CTR::IirFilterParamMic MicLpfParam[4] ={
		{
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 }			
		},

		{
			{ 0x1fc9, 0x1fc9, 0x1fc9, 0x0bc9, 0xe948 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 }			
		},

		{
			{ 0x10c7, 0x10c7, 0x10c7, 0x2fdb, 0xdd2b },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 }			
        },

		{
			{ 0x0a81, 0x0a81, 0x0a81, 0x42b1, 0xd094 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 },
			{ 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000 }			
		}
	};

	Result SetLowPassFilterCore( SamplingRate rate ){
		return detail::Mic::SetIirFilterMic( reinterpret_cast<const u8*>(&MicLpfParam[rate]), sizeof(nn::codec::CTR::IirFilterParamMic) );
	}
}

Result Initialize()  { return detail::InitializeBase(&detail::Mic::sSession, PORT_NAME_USER); }
Result Finalize()    { return detail::FinalizeBase(&detail::Mic::sSession); }

Result StartSampling(SamplingType type, SamplingRate rate, s32 offset, size_t size, bool loop){
	nn::Result result;
		
    if (!sIsBufferInitialized){
        return ResultNotInitialized();
    }

    if ((size & 0x1) || (offset & 0x1)){
        return ResultMisalignedSize();
    }

    if (offset + size > user_size){
        return ResultOutOfMemory();
    }

    if (sIsLpfEnable){
		result = SetLowPassFilterCore( rate );
	    if (result.IsFailure()){
			return result;
		}
	}

	result = detail::Mic::StartSampling( type, rate, offset, size, loop );
    if (result.IsSuccess()){
		sSamplingRate = rate; 
	}

    return result;
}

Result StopSampling(){
    return detail::Mic::StopSampling();
}

Result IsSampling(bool* pSampling){
    return detail::Mic::IsSampling( pSampling );
}

Result SetBuffer(void* p, size_t size){
    Result result;

    if (sIsBufferInitialized){
        return ResultAlreadyInitialized();
    }

    if (size & 0xFFF){
        return ResultMisalignedSize();
    }

    if ((u32)p & 0xFFF){
        return ResultMisalignedAddress();
    }

    //sSharedMemory.Initialize(p, size, os::MEMORY_PERMISSION_READ_WRITE);
    result = detail::Mic::AllocateBuffer(sSharedMemory.GetHandle(), size);

    user_size = size -sizeof(Header);
    if (result.IsSuccess()){ 
        sIsBufferInitialized = true;
    }

    return result;
}

Result ResetBuffer(){
    // TODO
}

Result GetSamplingBufferSize(uint size){
    // TODO
}

}
}
}
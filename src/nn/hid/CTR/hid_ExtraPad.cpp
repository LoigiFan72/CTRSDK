// Filename: hid_ExtraPad.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_ExtraPad.h>
#include <nn/ir/CTR/ir_CepdApi.h>

namespace nn{
namespace hid{
namespace CTR{

bool ExtraPad::IsSampling(){
    return nn::ir::CTR::CepdGetStatus() == nn::ir::CTR::CEPD_STATUS_SAMPLING;
}

}
}
}
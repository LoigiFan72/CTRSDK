#include "nn/Result.h"

namespace nn{
namespace fnd{
    enum Description{
        DESCRIPTION_INVALID_NODE                        = 1,
        DESCRIPTION_ALREADY_LISTED                      = 2,
        DESCRIPTION_OUT_OF_RANGE                        = 3,
        DESCRIPTION_OBSOLETE_RESULT                     = 1023
    };

    NN_DEFINE_RESULT_FUNC_LM(MakePermanentResult, Result::LEVEL_PERMANENT, Result::MODULE_NN_FND);
    NN_DEFINE_RESULT_CONST(MakeResultAlreadyListed,Result::LEVEL_USAGE, Result::SUMMARY_INVALID_ARGUMENT, Result::MODULE_NN_FND, nn::fnd::DESCRIPTION_ALREADY_LISTED);
    NN_DEFINE_RESULT_CONST(MakeResultInvalidNode,Result::LEVEL_USAGE, Result::SUMMARY_INVALID_ARGUMENT, Result::MODULE_NN_FND, nn::fnd::DESCRIPTION_INVALID_NODE);
    NN_DEFINE_RESULT_CONST(MakeResultInvalidAddress,Result::LEVEL_USAGE, Result::SUMMARY_INVALID_ARGUMENT, Result::MODULE_NN_FND, Result::DESCRIPTION_INVALID_ADDRESS);
    NN_DEFINE_RESULT_CONST(ResultOutOfRange,Result::LEVEL_USAGE, Result::SUMMARY_INVALID_ARGUMENT, Result::MODULE_NN_FND, nn::fnd::DESCRIPTION_OUT_OF_RANGE);
}
}
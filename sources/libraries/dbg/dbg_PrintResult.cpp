// Filename: dbg_PrintResult.cpp
//
// Project: Horizon

#include <nn/dbg/dbg_DebugString.h>

#include <nn/nstd/nstd_Printf.h>
#include <nn/module.h>
#include <nn/svc.h>

#include <string.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace nn{
namespace dbg{

#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)

void PrintResult(Result result){
    NN_TLOG_("Result (%08x)\n", result.GetPrintableBits());
    NN_TLOG_("Level:       (%4d) %s\n",result.GetLevel());
    NN_TLOG_("Summary:     (%4d) %s\n",result.GetSummary()); // GetSummaryString());
    NN_TLOG_("Module:      (%4d) %s\n",result.GetModule()); // GetModuleString());
    NN_TLOG_("Description: (%4d) %s\n",result.GetDescription()); // GetDescriptionString());
}

void PrintTResult(Result result){
    NN_TLOG_("Result (%08x)\n", result.GetPrintableBits());
    NN_TLOG_("Level:       (%4d) %s\n",result.GetLevel());
    NN_TLOG_("Summary:     (%4d) %s\n",result.GetSummary()); // GetSummaryString());
    NN_TLOG_("Module:      (%4d) %s\n",result.GetModule()); // GetModuleString());
    NN_TLOG_("Description: (%4d) %s\n",result.GetDescription()); // GetDescriptionString());
}

#else

void PrintResult(Result result){}
void PrintTResult(Result result){}

#endif

}
}
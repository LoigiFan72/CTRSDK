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
namespace detail{

void PrintResult(Result result){
    TPrintf("Result (%08x)\n", result.GetPrintableBits());
    TPrintf("Level:       (%4d) %s\n",result.GetLevel());
    TPrintf("Summary:     (%4d) %s\n",result.GetSummary()); // GetSummaryString());
    TPrintf("Module:      (%4d) %s\n",result.GetModule()); // GetModuleString());
    TPrintf("Description: (%4d) %s\n",result.GetDescription()); // GetDescriptionString());
}

}
}
}
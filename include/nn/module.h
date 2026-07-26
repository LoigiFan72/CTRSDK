#pragma once

#include <nn/util/detail/util_Symbol.h>

#define NN_MAKE_MODULE_STRING(Company, Module)          \
        "[SDK+" Company ":" Module "]"

#define NN_DEFINE_MODULE_ID_STRING(Variable, Value)     \
    const char Variable[] __attribute__((section(".module_id"))) = (Value)

#define NN_MAKE_MODULE(Variable, Company, Module)       \
        static NN_DEFINE_MODULE_ID_STRING(Variable, NN_MAKE_MODULE_STRING(Company, Module))

#define NN_REFER_MODULE(Variable)                       \
        NN_UTIL_REFER_SYMBOL(Variable)
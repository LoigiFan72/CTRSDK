#pragma once

typedef struct nnResult{
    bit32   value;
} nnResult;

namespace nn {

class Result {
private:
    static const bit32 MASK_FAIL_BIT        = 0x80000000u;

    static const s32 SIZE_DESCRIPTION       = 10;
    static const s32 SIZE_MODULE            =  8;
    static const s32 SIZE_RESERVE           =  3;
    static const s32 SIZE_SUMMARY           =  6;
    static const s32 SIZE_LEVEL             =  5;

    static const s32 SHIFTS_DESCRIPTION     = 0;
    static const s32 SHIFTS_MODULE          = SHIFTS_DESCRIPTION + SIZE_DESCRIPTION;
    static const s32 SHIFTS_RESERVE         = SHIFTS_MODULE      + SIZE_MODULE;
    static const s32 SHIFTS_SUMMARY         = SHIFTS_RESERVE     + SIZE_RESERVE;
    static const s32 SHIFTS_LEVEL           = SHIFTS_SUMMARY     + SIZE_SUMMARY;

    static const bit32 MASK_NEGATIVE_LEVEL  = (~0u) << SIZE_LEVEL;

#define NN_RESULT_H_MAKE_MASK(size, shift)  (((~0u) >> (32 - (size))) << (shift))
#define NN_RESULT_H_MAKE_MASK_HELPER(c)     static const bit32 MASK_ ## c = NN_RESULT_H_MAKE_MASK(SIZE_ ## c, SHIFTS_ ## c)
    NN_RESULT_H_MAKE_MASK_HELPER(DESCRIPTION);
    NN_RESULT_H_MAKE_MASK_HELPER(MODULE);
    NN_RESULT_H_MAKE_MASK_HELPER(SUMMARY);
    NN_RESULT_H_MAKE_MASK_HELPER(LEVEL);
#undef NN_RESULT_H_MAKE_MASK_HELPER
#undef NN_RESULT_H_MAKE_MASK

#define NN_RESULT_H_MAKE_MAX(size)          ((~0u) >> (32 - (size)))
#define NN_RESULT_H_MAKE_MAX_HELPER(c)      static const s32 MAX_ ## c = NN_RESULT_H_MAKE_MAX(SIZE_ ## c)
    NN_RESULT_H_MAKE_MAX_HELPER(DESCRIPTION);
    NN_RESULT_H_MAKE_MAX_HELPER(MODULE);
    NN_RESULT_H_MAKE_MAX_HELPER(SUMMARY);
    NN_RESULT_H_MAKE_MAX_HELPER(LEVEL);
#undef NN_RESULT_H_MAKE_MAX_HELPER
#undef NN_RESULT_H_MAKE_MAX

public:
    enum Level {
        LEVEL_INFO      =  1,
        LEVEL_SUCCESS   =  0,
        LEVEL_FATAL     = -1,
        LEVEL_RESET     = -2,
        LEVEL_REINIT    = -3,
        LEVEL_USAGE     = -4,
        LEVEL_PERMANENT = -5,
        LEVEL_TEMPORARY = -6,
        LEVEL_STATUS    = -7,
        LEVEL_END
    };

    enum Summary {
        SUMMARY_SUCCESS              =  0,
        SUMMARY_NOTHING_HAPPENED     =  1,
        SUMMARY_WOULD_BLOCK          =  2,
        SUMMARY_OUT_OF_RESOURCE      =  3,
        SUMMARY_NOT_FOUND            =  4,
        SUMMARY_INVALID_STATE        =  5,
        SUMMARY_NOT_SUPPORTED        =  6,
        SUMMARY_INVALID_ARGUMENT     =  7,
        SUMMARY_WRONG_ARGUMENT       =  8,
        SUMMARY_CANCELLED            =  9,
        SUMMARY_STATUS_CHANGED       = 10,
        SUMMARY_INTERNAL             = 11,
        SUMMARY_INVALID_RESULT_VALUE = MAX_SUMMARY
    };

    enum Module {
        MODULE_COMMON           = 0,
        MODULE_NN_KERNEL        = 1,
        MODULE_NN_UTIL,
        MODULE_NN_FILE_SERVER,
        MODULE_NN_LOADER_SERVER,
        MODULE_NN_TCB,
        MODULE_NN_OS,
        MODULE_NN_DBG,
        MODULE_NN_DMNT,
        MODULE_NN_PDN,
        MODULE_NN_GX,
        MODULE_NN_I2C,
        MODULE_NN_GPIO,
        MODULE_NN_DD,
        MODULE_NN_CODEC,
        MODULE_NN_SPI,
        MODULE_NN_PXI,
        MODULE_NN_FS,
        MODULE_NN_DI,
        MODULE_NN_HID,
        MODULE_NN_CAMERA,
        MODULE_NN_PI,
        MODULE_NN_PM,
        MODULE_NN_PMLOW,
        MODULE_NN_FSI,
        MODULE_NN_SRV,
        MODULE_NN_NDM,
        MODULE_NN_NWM,
        MODULE_NN_SOCKET,
        MODULE_NN_LDR,
        MODULE_NN_ACC,
        MODULE_NN_ROMFS,
        MODULE_NN_AM,
        MODULE_NN_HIO,
        MODULE_NN_UPDATER,
        MODULE_NN_MIC,
        MODULE_NN_FND,
        MODULE_NN_MP,
        MODULE_NN_MPWL,
        MODULE_NN_AC,
        MODULE_NN_HTTP,
        MODULE_NN_DSP,
        MODULE_NN_SND,
        MODULE_NN_DLP,
        MODULE_NN_HIOLOW,
        MODULE_NN_CSND,
        MODULE_NN_SSL,
        MODULE_NN_AMLOW,
        MODULE_NN_NEX,
        MODULE_NN_FRIENDS,
        MODULE_NN_RDT,
        MODULE_NN_APPLET,
        MODULE_NN_NIM,
        MODULE_NN_PTM,
        MODULE_NN_MIDI,
        MODULE_NN_MC,
        MODULE_NN_SWC,
        MODULE_NN_FATFS,
        MODULE_NN_NGC,
        MODULE_NN_CARD,
        MODULE_NN_CARDNOR,
        MODULE_NN_SDMC,
        MODULE_NN_BOSS,
        MODULE_NN_DBM,
        MODULE_NN_CFG,
        MODULE_NN_PS,
        MODULE_NN_CEC,
        MODULE_NN_IR,
        MODULE_NN_UDS,
        MODULE_NN_PL,
        MODULE_NN_CUP,
        MODULE_NN_GYROSCOPE,
        MODULE_NN_MCU,
        MODULE_NN_NS,
        MODULE_NN_NEWS,
        MODULE_NN_RO,
        MODULE_NN_GD,
        MODULE_NN_CARDSPI,
        MODULE_NN_EC,
        MODULE_NN_WEBBRS,
        MODULE_NN_TEST,
        MODULE_NN_ENC,
        MODULE_NN_PIA,
        MODULE_APPLICATION      = MAX_MODULE - 1,
        MODULE_INVALID_RESULT_VALUE = MAX_MODULE
    };

    enum Description {
        DESCRIPTION_SUCCESS             =                    0,
        DESCRIPTION_INVALID_SELECTION   = MAX_DESCRIPTION - 23,
        DESCRIPTION_TOO_LARGE           = MAX_DESCRIPTION - 22,
        DESCRIPTION_NOT_AUTHORIZED      = MAX_DESCRIPTION - 21,
        DESCRIPTION_ALREADY_DONE        = MAX_DESCRIPTION - 20,
        DESCRIPTION_INVALID_SIZE        = MAX_DESCRIPTION - 19,
        DESCRIPTION_INVALID_ENUM_VALUE  = MAX_DESCRIPTION - 18,
        DESCRIPTION_INVALID_COMBINATION = MAX_DESCRIPTION - 17,
        DESCRIPTION_NO_DATA             = MAX_DESCRIPTION - 16,
        DESCRIPTION_BUSY                = MAX_DESCRIPTION - 15,
        DESCRIPTION_MISALIGNED_ADDRESS  = MAX_DESCRIPTION - 14,
        DESCRIPTION_MISALIGNED_SIZE     = MAX_DESCRIPTION - 13,
        DESCRIPTION_OUT_OF_MEMORY       = MAX_DESCRIPTION - 12,
        DESCRIPTION_NOT_IMPLEMENTED     = MAX_DESCRIPTION - 11,
        DESCRIPTION_INVALID_ADDRESS     = MAX_DESCRIPTION - 10,
        DESCRIPTION_INVALID_POINTER     = MAX_DESCRIPTION -  9,
        DESCRIPTION_INVALID_HANDLE      = MAX_DESCRIPTION -  8,
        DESCRIPTION_NOT_INITIALIZED     = MAX_DESCRIPTION -  7,
        DESCRIPTION_ALREADY_INITIALIZED = MAX_DESCRIPTION -  6,
        DESCRIPTION_NOT_FOUND           = MAX_DESCRIPTION -  5,
        DESCRIPTION_CANCEL_REQUESTED    = MAX_DESCRIPTION -  4,
        DESCRIPTION_ALREADY_EXISTS      = MAX_DESCRIPTION -  3,
        DESCRIPTION_OUT_OF_RANGE        = MAX_DESCRIPTION -  2,
        DESCRIPTION_TIMEOUT             = MAX_DESCRIPTION -  1,
        DESCRIPTION_INVALID_RESULT_VALUE = MAX_DESCRIPTION
    };

    template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription> struct Const;
    template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription, int TDescriptionMin, int TDescriptionMax> struct ConstRange;
    template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule> struct Const_LSM;
    template <Result::Level TLevel, Result::Module TModule> struct Const_LM;
    template <Result::Level TLevel> struct Const_L;

public:
    bit32 mResult;
private:
    template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription> friend struct Const;
    template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription, int TDescriptionMin, int TDescriptionMax> friend struct ConstRange;
public:
    explicit Result(bit32 code) : mResult(code) {}
private:
    bit32 GetCodeBits(bit32 mask, s32 shift) const { return ((mResult & mask) >> shift); }

    template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription, int TDescriptionMin, int TDescriptionMax>
    bool operator==(ConstRange<TLevel, TSummary, TModule, TDescription, TDescriptionMin, TDescriptionMax>) const;
    template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription, int TDescriptionMin, int TDescriptionMax>
    bool operator!=(ConstRange<TLevel, TSummary, TModule, TDescription, TDescriptionMin, TDescriptionMax>) const;

public:
    Result()
        : mResult(static_cast<bit32>(
            ((static_cast<bit32>(LEVEL_USAGE)  << SHIFTS_LEVEL)       & MASK_LEVEL)       |
            ((SUMMARY_INVALID_RESULT_VALUE     << SHIFTS_SUMMARY)     & MASK_SUMMARY)     |
            ((MODULE_INVALID_RESULT_VALUE      << SHIFTS_MODULE)      & MASK_MODULE)      |
            ((DESCRIPTION_INVALID_RESULT_VALUE << SHIFTS_DESCRIPTION) & MASK_DESCRIPTION)))
    {}

    Result(Level level, Summary summary, Module module, int description)
        : mResult(static_cast<bit32>(
            ((level       << SHIFTS_LEVEL)       & MASK_LEVEL)       |
            ((summary     << SHIFTS_SUMMARY)     & MASK_SUMMARY)     |
            ((module      << SHIFTS_MODULE)      & MASK_MODULE)      |
            ((description << SHIFTS_DESCRIPTION) & MASK_DESCRIPTION)))
    {}

    bool IsFailure() const { return (mResult & MASK_FAIL_BIT) != 0; }
    bool IsSuccess() const { return !IsFailure(); }

    Level GetLevel() const {
        if(mResult & MASK_FAIL_BIT)
            return static_cast<Level>(GetCodeBits(MASK_LEVEL, SHIFTS_LEVEL) | MASK_NEGATIVE_LEVEL);
        return static_cast<Level>(GetCodeBits(MASK_LEVEL, SHIFTS_LEVEL));
    }

    Summary     GetSummary()     const { return static_cast<Summary>(GetCodeBits(MASK_SUMMARY, SHIFTS_SUMMARY)); }
    Module      GetModule()      const { return static_cast<Module>(GetCodeBits(MASK_MODULE, SHIFTS_MODULE)); }
    int         GetDescription() const { return static_cast<int>(GetCodeBits(MASK_DESCRIPTION, SHIFTS_DESCRIPTION)); }
    bit32       GetValue()       const { return mResult; }
    bit32       GetPrintableBits() const { return mResult; }

    Result(nnResult result) : mResult(result.value) {}
    operator nnResult() const{
        nnResult r = {mResult};
        return r;
    }

    bool operator==(const Result& rhs) const { return mResult == rhs.mResult; }
    bool operator!=(const Result& rhs) const { return mResult != rhs.mResult; }
};

template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription>
struct Result::Const : public Result {
    static const Result::Level   Level       = TLevel;
    static const Result::Summary Summary     = TSummary;
    static const Result::Module  Module      = TModule;
    static const int             Description = TDescription;

    static const bit32 Value = static_cast<bit32>(
        ((static_cast<bit32>(TLevel) << SHIFTS_LEVEL) & MASK_LEVEL)   |
        ((TSummary     << SHIFTS_SUMMARY)             & MASK_SUMMARY) |
        ((TModule      << SHIFTS_MODULE)              & MASK_MODULE)  |
        ((TDescription << SHIFTS_DESCRIPTION)         & MASK_DESCRIPTION));

    Const() : Result(Value) {}

    static bool Includes(Result result) {
        return result.GetModule() == TModule && result.GetDescription() == TDescription;
    }
};

template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule, int TDescription, int TDescriptionMin, int TDescriptionMax>
struct Result::ConstRange : public Result {
    static const Result::Level   Level          = TLevel;
    static const Result::Summary Summary        = TSummary;
    static const Result::Module  Module         = TModule;
    static const int             Description    = TDescription;
    static const int             DescriptionMin = TDescriptionMin;
    static const int             DescriptionMax = TDescriptionMax;

    static const bit32 Value = static_cast<bit32>(
        ((static_cast<bit32>(TLevel) << SHIFTS_LEVEL) & MASK_LEVEL)   |
        ((TSummary     << SHIFTS_SUMMARY)             & MASK_SUMMARY) |
        ((TModule      << SHIFTS_MODULE)              & MASK_MODULE)  |
        ((TDescription << SHIFTS_DESCRIPTION)         & MASK_DESCRIPTION));

    ConstRange() : Result(Value) {}

    static bool Includes(Result result) {
        return result.GetModule() == TModule &&
               TDescriptionMin <= result.GetDescription() &&
               result.GetDescription() <= TDescriptionMax;
    }

    friend bool operator<=(Result lhs, ConstRange) { return ConstRange::Includes(lhs); }
    friend bool operator>=(ConstRange, Result rhs)  { return ConstRange::Includes(rhs); }
private:
    bool operator==(Result) const;
    bool operator!=(Result) const;
};

template <Result::Level TLevel, Result::Summary TSummary, Result::Module TModule>
struct Result::Const_LSM : public Result {
    static const Result::Level   Level   = TLevel;
    static const Result::Summary Summary = TSummary;
    static const Result::Module  Module  = TModule;

    template <int TDescription>
    struct Const : public Result::Const<TLevel, TSummary, TModule, TDescription> {};

    Const_LSM(int description) : Result(TLevel, TSummary, TModule, description) {}
};

template <Result::Level TLevel, Result::Module TModule>
struct Result::Const_LM : public Result {
    static const Result::Level  Level  = TLevel;
    static const Result::Module Module = TModule;

    template <Result::Summary TSummary, int TDescription>
    struct Const : public Result::Const<TLevel, TSummary, TModule, TDescription> {};

    Const_LM(Result::Summary summary, int description) : Result(TLevel, summary, TModule, description) {}
};

template <Result::Level TLevel>
struct Result::Const_L : public Result {
    static const Result::Level Level = TLevel;

    template <Result::Summary TSummary, Result::Module TModule, int TDescription>
    struct Const : public Result::Const<TLevel, TSummary, TModule, TDescription> {};

    Const_L(Result::Summary summary, Result::Module module, int description) : Result(TLevel, summary, module, description) {}
};

#define NN_DEFINE_RESULT_CONST(name, level, summary, module, description) \
    typedef ::nn::Result::Const<(level), (summary), (module), (description)> name

#define NN_DEFINE_RESULT_CONST_RANGE(name, level, summary, module, description, descriptionMin, descriptionMax) \
    typedef ::nn::Result::ConstRange<(level), (summary), (module), (description), (descriptionMin), (descriptionMax)> name

#define NN_DEFINE_RESULT_FUNC_LSM(name, level, summary, module) \
    typedef ::nn::Result::Const_LSM<(level), (summary), (module)> name

#define NN_DEFINE_RESULT_CONST_LSM(name, f, description) \
    typedef f::Const<(description)> name

#define NN_DEFINE_RESULT_FUNC_LM(name, level, module) \
    typedef ::nn::Result::Const_LM<(level), (module)> name

#define NN_DEFINE_RESULT_CONST_LM(name, f, summary, description) \
    typedef f::Const<(summary), (description)> name

#define NN_DEFINE_RESULT_FUNC_L(name, level) \
    typedef ::nn::Result::Const_L<(level)> name

#define NN_DEFINE_RESULT_CONST_L(name, f, summary, module, description) \
    typedef f::Const<(summary), (module), (description)> name

NN_DEFINE_RESULT_CONST(ResultSuccess, Result::LEVEL_SUCCESS, Result::SUMMARY_SUCCESS, Result::MODULE_COMMON, Result::DESCRIPTION_SUCCESS);

inline Result MakeInfoResult     (Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_INFO,      summary, module, description); }
inline Result MakeFatalResult    (Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_FATAL,     summary, module, description); }
inline Result MakeResetResult    (Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_RESET,     summary, module, description); }
inline Result MakeReInitResult   (Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_REINIT,    summary, module, description); }
inline Result MakeUsageResult    (Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_USAGE,     summary, module, description); }
inline Result MakePermanentResult(Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_PERMANENT, summary, module, description); }
inline Result MakeTemporaryResult(Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_TEMPORARY, summary, module, description); }
inline Result MakeStatusResult   (Result::Summary summary, Result::Module module, int description) { return Result(Result::LEVEL_STATUS,    summary, module, description); }

} // namespace nn

typedef nn::Result Result;

#if NN_VERSION <= 2
    #ifdef __cplusplus
    extern "C" {
    #endif
        typedef void (*nnResultHandlerImpl)(nnResult result, const char* filename, int lineno, const char* fmt, va_list vlist);

        int nnResultFailureHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...);
        int nnResultTFailureHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...);

        int nnResultPanicHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...);
        int nnResultTPanicHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...);

        nnResult nnMakeInvalidResult();
    #ifdef __cplusplus
    }
    #endif
#endif
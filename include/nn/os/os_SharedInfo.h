#pragma once

#include <nn/os/os_Tick.h>

namespace nn{
namespace os{

struct RtcSwcInfo{
    s64 rtcBaseMilliSeconds;
    Tick rtcBaseTicks;
    s64 rtcTicksPerSecond;
    s64 rtcDiffMilliSeconds;
};

struct WritableSharedInfo{
    s32 rtcValidNumber;
    u8 targetHardware;
    u8 msuTargetInfo;
    u8 mcuInfoRegisters_10;
    bit8 reversed[25];
    RtcSwcInfo rtcSwcInfo[2];
    bit8 wirelessInfo[16];
    bit8 rev2[16];
    f32 svr2Volume;
    bool displayModeLockFlag;
    u8 batteryState;
    bit8 writeFlags;
    bit8 reversed88[25];
    ProgramId firstSystemMenu;
    ProgramId currentSystemMenu;
    bit8 reversedb0[16];
    bool isHeadphoneInserted;
};

struct ReadOnlySharedInfo{
    bit8 versionRservese;
    u8 versionMicro;
    u8 versionMinor;
    u8 versionMajor;
    bit32 flags; // FlagsEnum4 flags
    ProgramId firstMainProgram;
    bit32 coreVersion;
    bit8 deviceEnv;
    bit8 bootEnv;
    bit8 previousMode;
    s8 rev1;
    u32 revision;
    s32 rev2;
    bit32 kParamAck[4];
    bit32 kParamCurrent[4];
    bit32 kParamValues[8];
    bit8 firstVersionReserve;
};

inline ReadOnlySharedInfo& GetReadOnlySharedInfo(){ return *(ReadOnlySharedInfo*)NN_OS_ADDR_READONLY_SHARED_PAGE; }
inline WritableSharedInfo& GetWritableSharedInfo(){ return *(WritableSharedInfo*)NN_OS_ADDR_WRITABLE_SHARED_PAGE; }

}
}
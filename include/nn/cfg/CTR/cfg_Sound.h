#pragma once

namespace nn {
namespace cfg {
namespace CTR {
        enum CfgSoundOutputMode{
                CFG_SOUND_OUTPUT_MODE_MONO = 0,
                CFG_SOUND_OUTPUT_MODE_STEREO = 1,
                CFG_SOUND_OUTPUT_MODE_SURROUND = 2,
        };
namespace detail{
        struct SoundSettingCfgData{
                u8 soundOutputMode;
        };
}
}
}
}
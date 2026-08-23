#pragma once

namespace nn {
namespace ndm {
namespace CTR {

enum ExclusiveMode{
    EM_NONE,
    EM_INFRA,
    EM_LOCAL,
    EM_CEC,
    EM_INTENT_CEC
};

enum DaemonName{
    DN_CEC,
    DN_BOSS,
    DN_NIM,
    DN_FRIENDS,
    NUM_OF_DAEMONS
};

enum DaemonMask{
    DM_CEC = 1 << DN_CEC,
    DM_BOSS = 1 << DN_BOSS,
    DM_NIM = 1 << DN_NIM,
    DM_FRIENDS  = 1 << DN_FRIENDS
};

}
}
}
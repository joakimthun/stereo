#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        enum class Opcodes : u8
        {
            Ldstr = 0x72,
            Call = 0x28
        };

        struct Opcode
        {
            Opcodes code;
            std::wstring name;
        };

    }
}

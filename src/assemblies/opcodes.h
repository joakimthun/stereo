#pragma once

#include <string>
#include <array>

#include "../common/typedef.h"
#include "opcode.h"

namespace stereo {
    namespace assemblies {
        namespace opcodes {

            // If the first byte of the standard encoding is 0xFF, then the second byte can be 
            // used as 1 byte encoding. Otherwise the encoding is two bytes.
            const Opcode& get_one_byte_code(u8 index);
            const Opcode& get_two_byte_code(u8 index);
        }
    }
}

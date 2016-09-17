#pragma once

#include <intrin.h>

#include "typedef.h"

namespace stereo {
    namespace byteswap {

        u16 byteswap16(u16 value)
        {
            return _byteswap_ushort(value);
        }

        u32 byteswap32(u32 value)
        {
            return _byteswap_ulong(value);
        }

        u64 byteswap64(u64 value)
        {
            return _byteswap_uint64(value);
        }
    }
}
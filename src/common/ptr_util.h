#pragma once

#include "typedef.h"

namespace stereo {
    namespace common {

        inline u16 read16(const u8* ptr)
        {
            return *((u16*)ptr);
        }

        inline u32 read32(const u8* ptr)
        {
            return *((u32*)ptr);
        }

        inline u64 read64(const u8* ptr)
        {
            return *((u64*)ptr);
        }

    }
}
#pragma once

#include "typedef.h"

namespace stereo {
    namespace common {

        inline u16 peek16(const u8* ptr)
        {
            return *((u16*)ptr);
        }

        inline u32 peek32(const u8* ptr)
        {
            return *((u32*)ptr);
        }

        inline u64 peek64(const u8* ptr)
        {
            return *((u64*)ptr);
        }

        inline u8 read8(u8** ptr)
        {
            auto value = **ptr;
            *ptr += 1;

            return value;
        }

        inline u16 read16(u8** ptr)
        {
            auto value = peek16(*ptr);
            *ptr += 2;

            return value;
        }

        inline u32 read32(u8** ptr)
        {
            auto value = peek32(*ptr);
            *ptr += 4;

            return value;
        }

        inline u64 read64(u8** ptr)
        {
            auto value = peek64(*ptr);
            *ptr += 8;

            return value;
        }
    }
}
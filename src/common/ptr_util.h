#pragma once

#include "typedef.h"

namespace stereo {
    namespace ptrutil {

        inline u8 peek8(u8* ptr)
        {
            return *ptr;
        }

        inline u16 peek16(u8* ptr)
        {
            return *(reinterpret_cast<u16*>(ptr));
        }

        inline u32 peek32(u8* ptr)
        {
            return *(reinterpret_cast<u32*>(ptr));
        }

        inline u64 peek64(u8* ptr)
        {
            return *(reinterpret_cast<u64*>(ptr));
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
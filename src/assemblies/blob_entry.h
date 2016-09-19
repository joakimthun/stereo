#pragma once

#include "../common/typedef.h"

namespace stereo {
    namespace assemblies {

        struct BlobEntry
        {
            BlobEntry(u8* data, u32 length) : data(data), length(length) {}
            ~BlobEntry() { delete[] data; }
            u8* data;
            u32 length;
        };

    }
}

#pragma once

#include "../common/typedef.h"

namespace stereo {
    namespace runtime {

        enum class StereoObjectType
        {
            I32,
            I64
        };

        union StereoValue
        {
            i32 i32;
            i64 i64;
        };

        struct StereoObject
        {
            StereoValue value;
            StereoObjectType type;
        };
    }
}
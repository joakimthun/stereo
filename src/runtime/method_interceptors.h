#pragma once

#include <string>

#include "../common/typedef.h"

namespace stereo {
    namespace runtime {

        class StereoObject;

        bool try_call_interceptor(const std::wstring& fullname, StereoObject* stack, u32& sp);

    }
}
#pragma once

#include "stereo_exception.h"
#include "../common/str_util.h"

namespace stereo {
    namespace exceptions {

        StereoException::StereoException(const std::wstring& message) : message_(message) {}

        const std::wstring& StereoException::msg() const
        {
            return message_;
        }

    }
}
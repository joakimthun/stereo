#pragma once

#include "stereo_exception.h"
#include "../common/str_util.h"

namespace stereo {
    namespace exceptions {

        StereoException::StereoException(const std::string& message) : message_(message) {}

        const char * StereoException::what() const throw()
        {
            return message_.c_str();
        }

    }
}
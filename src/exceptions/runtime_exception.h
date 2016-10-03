#pragma once

#include "stereo_exception.h"

namespace stereo {
    namespace exceptions {

        class RuntimeException : public StereoException
        {
        public:
            inline RuntimeException(const std::wstring& message) : StereoException(message) {}
        };

    }
}

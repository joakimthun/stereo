#pragma once

#include "stereo_exception.h"

namespace stereo {
    namespace exceptions {

        class NotImplementedException : public StereoException
        {
        public:
            inline NotImplementedException(const std::wstring& message) : StereoException(message) {}
        };

    }
}

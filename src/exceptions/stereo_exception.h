#pragma once

#include <exception>
#include <string>

namespace stereo {
    namespace exceptions {

        class StereoException : public std::exception
        {
        public:
            StereoException(const std::wstring& message);

            const std::wstring& StereoException::msg() const;

        protected:
            std::wstring message_;
        };

    }
}
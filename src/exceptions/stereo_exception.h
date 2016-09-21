#pragma once

#include <exception>
#include <string>

namespace stereo {
    namespace exceptions {

        class StereoException : public std::exception
        {
        public:
            StereoException(const std::string& message);

            virtual const char* what() const throw();

        protected:
            std::string message_;
        };

    }
}
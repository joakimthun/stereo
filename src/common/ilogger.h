#pragma once

#include <string>

namespace stereo {
    namespace common {

        enum class LogLevel
        {
            Info = 0,
            Warning = 1,
            Error = 2
        };

        class ILogger
        {
        public:
            virtual void Log(LogLevel log_level, const std::wstring& message) = 0;
            virtual void LogInfo(const std::wstring& message) = 0;
            virtual void LogWarning(const std::wstring& message) = 0;
            virtual void LogError(const std::wstring& message) = 0;

        protected:
            static const std::wstring& get_log_level_name(LogLevel log_level);
        };

    }
}
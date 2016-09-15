#pragma once

#include "ilogger.h"

namespace stereo {
    namespace common {

        class ConsoleLogger : public ILogger
        {
        public:
            void Log(LogLevel log_level, const std::wstring& message) override;
            void LogInfo(const std::wstring& message) override;
            void LogWarning(const std::wstring& message) override;
            void LogError(const std::wstring& message) override;

        private:
            void write(const std::wstring& log_level, const std::wstring& message);
        };

    }
}
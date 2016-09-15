#pragma once

#include <iostream>

#include "console_logger.h"

namespace stereo {
    namespace common {

        void ConsoleLogger::Log(LogLevel log_level, const std::wstring& message)
        {
            const auto& name = ILogger::get_log_level_name(log_level);
            write(name, message);
        }

        void ConsoleLogger::LogInfo(const std::wstring& message)
        {
            Log(LogLevel::Info, message);
        }

        void ConsoleLogger::LogWarning(const std::wstring& message)
        {
            Log(LogLevel::Warning, message);
        }

        void ConsoleLogger::LogError(const std::wstring& message)
        {
            Log(LogLevel::Error, message);
        }

        void ConsoleLogger::write(const std::wstring& log_level, const std::wstring& message)
        {
            std::wcout << log_level << ":	" << message << std::endl;
        }
    }
}
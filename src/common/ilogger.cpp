#pragma once

#include "ilogger.h"

namespace stereo {
    namespace common {

        static std::wstring log_level_names[] = {
            L"Info",
            L"Warning",
            L"Error"
        };

        const std::wstring& ILogger::get_log_level_name(LogLevel log_level)
        {
            return log_level_names[static_cast<int>(log_level)];
        }

    }
}
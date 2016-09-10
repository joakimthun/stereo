#pragma once

#include "ilogger.h"

namespace stereo {

	static std::string log_level_names[] = {
		"Info",
		"Warning",
		"Error"
	};

	const std::string& ILogger::get_log_level_name(LogLevel log_level)
	{
		return log_level_names[static_cast<int>(log_level)];
	}

}
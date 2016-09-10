#pragma once

#include <iostream>

#include "console_logger.h"

namespace stereo {

	void ConsoleLogger::Log(LogLevel log_level, const std::string& message)
	{
		const auto& name = ILogger::get_log_level_name(log_level);
		write(name, message);
	}

	void ConsoleLogger::LogInfo(const std::string& message)
	{
		Log(LogLevel::Info, message);
	}

	void ConsoleLogger::LogWarning(const std::string& message)
	{
		Log(LogLevel::Warning, message);
	}

	void ConsoleLogger::LogError(const std::string& message)
	{
		Log(LogLevel::Error, message);
	}

	void ConsoleLogger::write(const std::string& log_level, const std::string& message)
	{
		std::cout << log_level << ":	" << message << std::endl;
	}

}
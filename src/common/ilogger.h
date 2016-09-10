#pragma once

#include <string>

namespace stereo {

	enum class LogLevel
	{
		Info = 0,
		Warning = 1,
		Error = 2
	};

	class ILogger
	{
	public:
		virtual void Log(LogLevel log_level, const std::string& message) = 0;
		virtual void LogInfo(const std::string& message) = 0;
		virtual void LogWarning(const std::string& message) = 0;
		virtual void LogError(const std::string& message) = 0;

	protected:
		static const std::string& get_log_level_name(LogLevel log_level);
	};

}
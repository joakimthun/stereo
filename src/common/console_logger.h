#pragma once

#include "ilogger.h"

namespace stereo {

	class ConsoleLogger : public ILogger
	{
	public:
		void Log(LogLevel log_level, const std::string& message) override;
		void LogInfo(const std::string& message) override;
		void LogWarning(const std::string& message) override;
		void LogError(const std::string& message) override;

	private:
		void write(const std::string& log_level, const std::string& message);
	};

}
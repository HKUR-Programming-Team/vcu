#include <UtilsLib/Inc/Logger.hpp>

namespace VehicleControlUnit { namespace UtilsLib {

void Logger::LogSpam(const std::string & logMessage) const
{
	PrintLogMsg("SPAM", logMessage);
}

void Logger::LogInfo(const std::string & logMessage) const
{
	PrintLogMsg("INFO", logMessage);
}

void Logger::LogError(const std::string & logMessage) const
{
	PrintLogMsg("ERROR", logMessage);
}

void Logger::PrintLogMsg(const std::string & logType, const std::string & logMessage) const
{
	if (logType == "SPAM")
	{
		if (!spamLoggingEnabled) return;
	}
	else if (logType == "INFO")
	{
		if (!infoLoggingEnabled) return;
	}
	else if (logType == "ERROR")
	{
		if (!errorLoggingEnabled) return;
	}
	else
	{
		return; //ERROR
	}

	const auto currentTick = HAL_GetTick();
	// TODO: add current tick time to the log message
	printf("%d [%s] %s\n", currentTick, logType.c_str(), logMessage.c_str());
}

}}

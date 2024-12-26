#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string>

#include <stm32f1xx.h>

namespace VehicleControlUnit::UtilsLib {

class Logger
{
public:
	// Set whether each type of logs are enabled
	Logger(const bool spamLoggingEnabled, const bool infoLoggingEnabled,
			const bool errorLoggingEnabled, const bool customLoggingEnabled):
		spamLoggingEnabled(spamLoggingEnabled),
		infoLoggingEnabled(infoLoggingEnabled),
		errorLoggingEnabled(errorLoggingEnabled),
		customLoggingEnabled(customLoggingEnabled)
	{}

	// Interfaces for logging
	void LogSpam(const std::string & logMessage) const;
	void LogInfo(const std::string & logMessage) const;
	void LogError(const std::string & logMessage) const;
	void LogCustom(const std::string & logMessage) const;
private:
	void PrintLogMsg(const std::string & logType, const std::string & logMessage) const;

	bool spamLoggingEnabled;
	bool infoLoggingEnabled;
	bool errorLoggingEnabled;
	bool customLoggingEnabled;
};

} // namespace VehicleControlUnit::UtilsLib


#pragma once

#include <stm32f1xx.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>

namespace VehicleControlUnit { namespace UtilsLib {

class Logger
{
public:
	// Set whether each type of logs are enabled
	Logger(const bool enableSpamLog, const bool enableInfoLog, const bool enableErrorLog, const bool enableCustomLog):
		spamLoggingEnabled(enableSpamLog),
		infoLoggingEnabled(enableInfoLog),
		errorLoggingEnabled(enableErrorLog),
		customLoggingEnabled(enableCustomLog)
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

}} // namespace VehicleControlUnit::UtilsLib


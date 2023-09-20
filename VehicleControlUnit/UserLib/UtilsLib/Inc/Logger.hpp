#ifndef VCU_LOGGER
#define VCU_LOGGER

#include <stm32f1xx.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>

namespace VehicleControlUnit { namespace UtilsLib {

class Logger
{
public:
	// Set whether each type of logs are enabled
	Logger(const bool enableSpamLog, const bool enableInfoLog, const bool enableErrorLog):
		spamLoggingEnabled(enableSpamLog),
		infoLoggingEnabled(enableInfoLog),
		errorLoggingEnabled(enableErrorLog)
	{}

	// Interfaces for logging
	void LogSpam(const std::string & logMessage) const;
	void LogInfo(const std::string & logMessage) const;
	void LogError(const std::string & logMessage) const;
private:
	void PrintLogMsg(const std::string & logType, const std::string & logMessage) const;

	bool spamLoggingEnabled;
	bool infoLoggingEnabled;
	bool errorLoggingEnabled;
};

}} // namespace VehicleControlUnit::UtilsLib

#endif

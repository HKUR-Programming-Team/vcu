#pragma once

#include <MainLib/Inc/Config.hpp>
#include <MainLib/Inc/ConfigValueParser.hpp>
#include <MainLib/Inc/json.hpp>

#include <optional>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif

using json = nlohmann::json;

namespace VehicleControlUnit::MainLib::Config {

class ConfigGroupParser
{

public:
static std::optional<LoggerConfig> ParseLoggerParameteres(const json& config);
static std::optional<ErrorConfig> ParseErrorConfig(const json& config);
static std::optional<ReadyToDriveConfig> ParseReadyToDriveConfig(const json& config);
static std::optional<SensorInterfaceThrottleConfig> ParseSensorInterfaceThrottleConfig(const json& config);
static std::optional<SensorInterfaceBrakeConfig> ParseSensorInterfaceBrakeConfig(const json& config);
static std::optional<SensorInterfaceRegenConfig> ParseSensorInterfaceRegenConfig(const json& config);

};

}
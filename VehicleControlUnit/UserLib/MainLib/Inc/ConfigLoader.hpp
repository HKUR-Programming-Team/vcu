#pragma once

#include <MainLib/Inc/Config.hpp>
#include <MainLib/Inc/json.hpp>

#include <optional>

using json = nlohmann::json;

namespace VehicleControlUnit::MainLib::Config {

class ConfigLoader{
public:
Config GetConfig()
{
    return Config();
}

private:
std::optional<std::string> LoadConfigFromMemory();

std::optional<Config> ParseConfig(const json ConfigJson);
std::optional<LoggerConfig> ParseLoggerParameteres();
std::optional<ADCConfig> ParsedcConfig();
std::optional<ErrorConfig> ParseErrorConfig();
std::optional<ReadyToDriveConfig> ParseReadyToDriveConfig();
std::optional<SensorInterfaceConfig> ParseSensorInterfaceConfig();
std::optional<MCUInterfaceConfig> ParseMcuInterfaceConfig();
std::optional<DashboardInterfaceConfig> ParseDashboardInterfaceConfig();

LoggerConfig mLoggerConfig;
ADCConfig mAdcConfig;
ErrorConfig mErrorConfig;
ReadyToDriveConfig mReadyToDriveConfig;
SensorInterfaceConfig mSensorInterfaceConfig;
MCUInterfaceConfig mMcuInterfaceConfig;
DashboardInterfaceConfig mDashboardInterfaceConfig;
};

}
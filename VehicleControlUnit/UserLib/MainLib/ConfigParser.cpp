#include <MainLib/Inc/ConfigParser.hpp>

namespace VehicleControlUnit::MainLib::Config {

std::optional<Config> ConfigParser::Parse(const json& configJson) const
{
	if (!configJson.is_object()) {
		printf("[Config Loader]: configJson is not an object.\n");
		return std::nullopt;
	}

	// Check config version
	const auto versionOpt = ConfigValueParser::GetString(configJson, "version");
	if (!versionOpt.has_value())
	{
		return std::nullopt;
	}

	const auto version = versionOpt.value();
	if (version != mSupportedVersion)
	{
		printf("[Config Loader]: version unsupported. Supported version: %s, config version: %s.\n", mSupportedVersion.c_str(), version.c_str());
		return std::nullopt;
	}

	// Check for success of each parse
	const auto logger = ConfigGroupParser::ParseLoggerParameteres(configJson);
	const auto error = ConfigGroupParser::ParseErrorConfig(configJson);
	const auto readyToDrive = ConfigGroupParser::ParseReadyToDriveConfig(configJson);
	const auto sensorInterfaceThrottle = ConfigGroupParser::ParseSensorInterfaceThrottleConfig(configJson);
	const auto sensorInterfaceBrake = ConfigGroupParser::ParseSensorInterfaceBrakeConfig(configJson);
	const auto sensorInterfaceRegen = ConfigGroupParser::ParseSensorInterfaceRegenConfig(configJson);

	if (!logger.has_value()
			|| !error.has_value()
			|| !readyToDrive.has_value()
			|| !sensorInterfaceThrottle.has_value()
			|| !sensorInterfaceBrake.has_value()
			|| !sensorInterfaceRegen.has_value())
	{
		return std::nullopt;
	}

	printf("[Config Loader]: config parse is successful.\n");

	// The following uses default values
	const ADCConfig adcConfig;
	const MCUInterfaceConfig mcuConfig;
	const DashboardInterfaceConfig dashboardConfig;

	// If all parses succeed, return the config.
	return Config(logger.value(),
			adcConfig,
			error.value(),
			readyToDrive.value(),
			sensorInterfaceThrottle.value(),
			sensorInterfaceBrake.value(),
			sensorInterfaceRegen.value(),
			mcuConfig,
			dashboardConfig);
}

}

#include <MainLib/Inc/ConfigGroupParser.hpp>

namespace VehicleControlUnit::MainLib::Config {

std::optional<LoggerConfig> ConfigGroupParser::ParseLoggerParameteres(const json& config)
{
	const auto spamLoggingEnabledOpt = ConfigValueParser::GetBool(config, "spamLoggingEnabled");
	const auto infoLoggingEnabledOpt = ConfigValueParser::GetBool(config, "infoLoggingEnabled");
	const auto errorLoggingEnabledOpt = ConfigValueParser::GetBool(config, "errorLoggingEnabled");
	const auto customLoggingEnabledOpt = ConfigValueParser::GetBool(config, "customLoggingEnabled");

    if (!spamLoggingEnabledOpt.has_value()
            || !infoLoggingEnabledOpt.has_value()
            || !errorLoggingEnabledOpt.has_value()
            || !customLoggingEnabledOpt.has_value())
    {
        return std::nullopt;
    }

	return LoggerConfig {
		spamLoggingEnabledOpt.value(),
		infoLoggingEnabledOpt.value(),
		errorLoggingEnabledOpt.value(),
		customLoggingEnabledOpt.value()
	};
}

std::optional<ErrorConfig> ConfigGroupParser::ParseErrorConfig(const json& config)
{
	const auto implausibleThresholdIntervalOpt = ConfigValueParser::GetU32(config, "implausibleThresholdInterval");

	if (!implausibleThresholdIntervalOpt.has_value())
	{
		return std::nullopt;
	}

	return ErrorConfig {
		implausibleThresholdIntervalOpt.value()
	};
}

std::optional<ReadyToDriveConfig> ConfigGroupParser::ParseReadyToDriveConfig(const json& config)
{
	const auto readyToDriveSoundDurationOpt = ConfigValueParser::GetU32(config, "readyToDriveSoundDuration");
	const auto readyToDriveTriggeringBrakeThresholdOpt = ConfigValueParser::GetU16(config, "readyToDriveTriggeringBrakeThreshold");
	const auto readyToDriveButtonPortOpt = ConfigValueParser::GetGPIOPort(config, "readyToDriveButtonPort");
	const auto readyToDriveButtonPinNumOpt = ConfigValueParser::GetGPIOPinNum(config, "readyToDriveButtonPinNum");
	const auto readyToDriveSoundPortOpt = ConfigValueParser::GetGPIOPort(config, "readyToDriveSoundPort");
	const auto readyToDriveSoundPinNumOpt = ConfigValueParser::GetGPIOPinNum(config, "readyToDriveSoundPinNum");

    if (!readyToDriveSoundDurationOpt.has_value()
            || !readyToDriveTriggeringBrakeThresholdOpt.has_value()
            || !readyToDriveButtonPortOpt.has_value()
            || !readyToDriveButtonPinNumOpt.has_value()
            || !readyToDriveSoundPortOpt.has_value()
            || !readyToDriveSoundPinNumOpt.has_value())
    {
        return std::nullopt;
    }

	return ReadyToDriveConfig {
		readyToDriveSoundDurationOpt.value(),
		readyToDriveTriggeringBrakeThresholdOpt.value(),
		readyToDriveButtonPortOpt.value(),
		readyToDriveButtonPinNumOpt.value(),
		readyToDriveSoundPortOpt.value(),
		readyToDriveSoundPinNumOpt.value()
	};
}

std::optional<SensorInterfaceThrottleConfig> ConfigGroupParser::ParseSensorInterfaceThrottleConfig(const json& config)
{
    const auto ThrottleMinPin0Opt = ConfigValueParser::GetU16(config, "ThrottleMinPin0");
    const auto ThrottleMaxPin0Opt = ConfigValueParser::GetU16(config, "ThrottleMaxPin0");
    const auto ThrottleMinPin1Opt = ConfigValueParser::GetU16(config, "ThrottleMinPin1");
    const auto ThrottleMaxPin1Opt = ConfigValueParser::GetU16(config, "ThrottleMaxPin1");
    const auto MaxTorqueOpt = ConfigValueParser::GetI16(config, "MaxTorque");
    const auto ThrottleSignalOutOfRangeThresholdOpt = ConfigValueParser::GetU16(config, "ThrottleSignalOutOfRangeThreshold");
    const auto ThrottleSignalDeviationThresholdOpt = ConfigValueParser::GetU16(config, "ThrottleSignalDeviationThreshold");
    const auto SignalDeadzoneOpt = ConfigValueParser::GetU16(config, "SignalDeadzone");

    if(!ThrottleMinPin0Opt.has_value()
        || !ThrottleMaxPin0Opt.has_value()
        || !ThrottleMinPin1Opt.has_value()
        || !ThrottleMaxPin1Opt.has_value()
        || !MaxTorqueOpt.has_value()
        || !ThrottleSignalOutOfRangeThresholdOpt.has_value()
        || !ThrottleSignalDeviationThresholdOpt.has_value()
        || !SignalDeadzoneOpt.has_value())
    {
        return std::nullopt;
    }

    // Use default value for the ADC Indices.
    SensorInterfaceThrottleConfig throttleConfig;
    throttleConfig.ThrottleMinPin0 = ThrottleMinPin0Opt.value();
    throttleConfig.ThrottleMaxPin0 = ThrottleMaxPin0Opt.value();
    throttleConfig.ThrottleMinPin1 = ThrottleMinPin1Opt.value();
    throttleConfig.ThrottleMaxPin1 = ThrottleMaxPin1Opt.value();
    throttleConfig.MaxTorque = MaxTorqueOpt.value();
    throttleConfig.ThrottleSignalOutOfRangeThreshold = ThrottleSignalOutOfRangeThresholdOpt.value();
    throttleConfig.ThrottleSignalDeviationThreshold = ThrottleSignalDeviationThresholdOpt.value();
    throttleConfig.SignalDeadzone = SignalDeadzoneOpt.value();

    return throttleConfig;
}

std::optional<SensorInterfaceBrakeConfig> ConfigGroupParser::ParseSensorInterfaceBrakeConfig(const json& config)
{
    const auto BrakeMinPinOpt = ConfigValueParser::GetU16(config, "BrakeMinPin");
    const auto BrakeMaxPinOpt = ConfigValueParser::GetU16(config, "BrakeMaxPin");
    const auto BrakeSignalOutOfRangeThresholdOpt = ConfigValueParser::GetU16(config, "BrakeSignalOutOfRangeThreshold");
    const auto MaxBrakeOpt = ConfigValueParser::GetU16(config, "MaxBrake");

    if(!BrakeMinPinOpt.has_value()
            || !BrakeMaxPinOpt.has_value()
            || !BrakeSignalOutOfRangeThresholdOpt.has_value()
            || !MaxBrakeOpt.has_value())
    {
        return std::nullopt;
    }

    // Use default value for the ADC Indices.
    SensorInterfaceBrakeConfig brakeConfig;
    brakeConfig.BrakeMinPin = BrakeMinPinOpt.value();
    brakeConfig.BrakeMaxPin = BrakeMaxPinOpt.value();
    brakeConfig.BrakeSignalOutOfRangeThreshold = BrakeSignalOutOfRangeThresholdOpt.value();
    brakeConfig.MaxBrake = MaxBrakeOpt.value();

    return brakeConfig;
}

std::optional<SensorInterfaceRegenConfig> ConfigGroupParser::ParseSensorInterfaceRegenConfig(const json& config)
{
    const auto RegenMinPinOpt = ConfigValueParser::GetU16(config, "RegenMinPin");
    const auto RegenMaxPinOpt = ConfigValueParser::GetU16(config, "RegenMaxPin");
    const auto MaxRegenOpt = ConfigValueParser::GetI16(config, "MaxRegen");

    if(!RegenMinPinOpt.has_value()
            || !RegenMaxPinOpt.has_value()
            || !MaxRegenOpt.has_value())
    {
        return std::nullopt;
    }

    // Use default value for the ADC Indices.
    SensorInterfaceRegenConfig regenConfig;
    regenConfig.RegenMinPin = RegenMinPinOpt.value();
    regenConfig.RegenMaxPin = RegenMaxPinOpt.value();
    regenConfig.MaxRegen = MaxRegenOpt.value();

    return regenConfig;
}

}
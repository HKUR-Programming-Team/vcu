#pragma once

#ifndef MOCK_TEST
	#include <UtilsLib/Inc/GPIOManager.hpp>
#else
	#include <MockLibraries.hpp>
#endif

namespace VehicleControlUnit::MainLib::Config {

struct LoggerConfig
{
	const bool spamLoggingEnabled = false;
	const bool infoLoggingEnabled = true;
	const bool errorLoggingEnabled = true;
	const bool customLoggingEnabled = false;
};

struct ADCConfig
{
	uint32_t ADCDMABufferLength = 5;
};

struct ErrorConfig
{
	uint32_t implausibleThresholdInterval = 100;
};

// Ready to drive sound
struct ReadyToDriveConfig
{ 
	uint32_t readyToDriveSoundDuration = 1500;
	uint16_t readyToDriveTriggeringBrakeThreshold = 20;
	UtilsLib::GPIOPort readyToDriveButtonPort = UtilsLib::GPIOPort::C;
	UtilsLib::GPIOPinNum readyToDriveButtonPinNum = UtilsLib::GPIOPinNum::Pin7;
	UtilsLib::GPIOPort readyToDriveSoundPort = UtilsLib::GPIOPort::C;
	UtilsLib::GPIOPinNum readyToDriveSoundPinNum = UtilsLib::GPIOPinNum::Pin1;
};

// Sensor Interface
struct SensorInterfaceConfig
{
    uint16_t ThrottleMinPin0 = 500;
	uint16_t ThrottleMaxPin0 = 3500;
	uint16_t ThrottleMinPin1 = 500;
	uint16_t ThrottleMaxPin1 = 3500;
	int16_t MaxTorque = 1660;

	uint16_t ThrottleSignalOutOfRangeThreshold = 20;
	uint16_t ThrottleSignalDeviationThreshold = MaxTorque / 10; // Maximum 10% deviation allowed. FSUK2024 T11.8.9
	uint16_t SignalDeadzone = 50;

	uint8_t ThrottleSignalADCIndex1 = 0;
    uint8_t ThrottleSignalADCIndex2 = 1;

	uint8_t BrakeSignalADCIndex = 0;
	uint16_t BrakeMinPin = 500;
	uint16_t BrakeMaxPin = 3500;
	uint16_t BrakeSignalOutOfRangeThreshold = 30;
	uint16_t MaxBrake = 100;

	uint8_t RegenSignalADCIndex = 0;
	uint16_t RegenMinPin = 500;
	uint16_t RegenMaxPin = 3500;
	int16_t MaxRegen = 500;
};

struct MCUInterfaceConfig
{
	uint32_t CommandMessageHeaderId = 0x0C0;
	uint32_t CommandMessageLength = 8;
	uint32_t CommandMessageTransmitInterval = 3;
	int16_t InverterEnableTorqueThreshold = 10;
	int16_t RegenEnableTorqueThreshold = 10;
	double WheelRadius = 0.2;
	double MinimumTCSTriggeringLinearSpeed = 1.0;
	double TCSTriggeringSlipRatioThreshold = 1;
	double TCSHaltSlipRatioThreshold = 0.5;
};

struct DashboardInterfaceConfig
{
	UtilsLib::GPIOPort ImplausibleErrorLightPort = UtilsLib::GPIOPort::A;
	UtilsLib::GPIOPinNum ImplausibleErrorLightPinNum = UtilsLib::GPIOPinNum::Pin9;
	UtilsLib::GPIOPort ThrottleSignalErrorLightPort = UtilsLib::GPIOPort::B;
	UtilsLib::GPIOPinNum ThrottleSignalErrorLightPinNum = UtilsLib::GPIOPinNum::Pin13;
	UtilsLib::GPIOPort BrakeSignalErrorLightPort = UtilsLib::GPIOPort::B;
	UtilsLib::GPIOPinNum BrakeSignalErrorLightPinNum = UtilsLib::GPIOPinNum::Pin12;
	UtilsLib::GPIOPort MCUTimeoutErrorLightPort = UtilsLib::GPIOPort::B;
	UtilsLib::GPIOPinNum MCUTimeoutErrorLightPinNum = UtilsLib::GPIOPinNum::Pin11;
};

class Config {
public:
Config(LoggerConfig loggerConfig,
    ADCConfig adcConfig,
    ErrorConfig errorConfig,
    ReadyToDriveConfig readyToDriveConfig,
    SensorInterfaceConfig sensorInterfaceConfig,
    MCUInterfaceConfig mcuInterfaceConfig,
    DashboardInterfaceConfig dashboardInterfaceConfig):
        mLoggerConfig(loggerConfig),
        mAdcConfig(adcConfig),
        mErrorConfig(errorConfig),
        mReadyToDriveConfig(readyToDriveConfig),
        mSensorInterfaceConfig(sensorInterfaceConfig),
        mMcuInterfaceConfig(mcuInterfaceConfig),
        mDashboardInterfaceConfig(dashboardInterfaceConfig)
{}

Config(){
    // Use this constructor when the default values in the config is used.
}

LoggerConfig mLoggerConfig;
ADCConfig mAdcConfig;
ErrorConfig mErrorConfig;
ReadyToDriveConfig mReadyToDriveConfig;
SensorInterfaceConfig mSensorInterfaceConfig;
MCUInterfaceConfig mMcuInterfaceConfig;
DashboardInterfaceConfig mDashboardInterfaceConfig;
};

}
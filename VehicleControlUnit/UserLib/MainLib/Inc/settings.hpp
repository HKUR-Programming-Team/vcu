#pragma once

#ifndef MOCK_TEST
	#include <UtilsLib/Inc/GPIOManager.hpp>
#else
	#include <MockLibraries.hpp>
#endif

namespace VehicleControlUnit::MainLib::Settings {

// Logger
const bool spamLoggingEnabled = false;
const bool infoLoggingEnabled = true;
const bool errorLoggingEnabled = true;
const bool customLoggingEnabled = false;

// MCU
const uint32_t implausibleThresholdInterval = 100;

// ADC
const uint32_t ADCDMABufferLength = 5;

// Ready to drive sound
struct ReadyToDriveParameters
{ 
	uint32_t readyToDriveSoundDuration = 1500;
	uint16_t readyToDriveTriggeringBrakeThreshold = 20;
	UtilsLib::GPIOPort readyToDriveButtonPort = UtilsLib::GPIOPort::C;
	UtilsLib::GPIOPinNum readyToDriveButtonPinNum = UtilsLib::GPIOPinNum::Pin7;
	UtilsLib::GPIOPort readyToDriveSoundPort = UtilsLib::GPIOPort::C;
	UtilsLib::GPIOPinNum readyToDriveSoundPinNum = UtilsLib::GPIOPinNum::Pin1;
};
const ReadyToDriveParameters readyToDriveParameters;

// Sensor Interface
struct SensorInterfaceParameters
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
const SensorInterfaceParameters sensorInterfaceParameters;

struct MCUInterfaceParameters
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
const MCUInterfaceParameters mcuInterfaceParameters;

struct DashboardInterfaceParameters
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
const DashboardInterfaceParameters dashboardInterfaceParameters;

} // namespace VehicleControlUnit::MainLib::Settings


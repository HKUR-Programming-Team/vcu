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

// ADC (Move these to ADC manager?)
const uint32_t ADCDMABufferLength = 3;
const uint8_t throttleSignalADCIndex1 = 0;
const uint8_t throttleSignalADCIndex2 = 1;

// Ready to drive sound
struct ReadyToDriveParameters
{
	uint32_t readyToDriveSoundDuration = 1500;
	uint16_t readyToDriveTriggeringBrakeThreshold = 20;
	UtilsLib::GPIOPort readyToDriveButtonPort = UtilsLib::GPIOPort::A;
	UtilsLib::GPIOPinNum readyToDriveButtonPinNum = UtilsLib::GPIOPinNum::Pin0;
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

} // namespace VehicleControlUnit::MainLib::Settings


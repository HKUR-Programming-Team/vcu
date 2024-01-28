#pragma once

namespace VehicleControlUnit::MainLib::Settings {

// Logger
const bool spamLoggingEnabled = false;
const bool infoLoggingEnabled = false;
const bool errorLoggingEnabled = false;
const bool customLoggingEnabled = true;

// MCU
const uint32_t implausibleThresholdInterval = 100;

// ADC (Move these to ADC manager?)
const uint32_t ADCDMABufferLength = 3;
const uint8_t throttleSignalADCIndex1 = 0;
const uint8_t throttleSignalADCIndex2 = 1;

// Ready to drive sound
const uint32_t readyToDriveSoundDuration = 1500;

// Sensor Interface
struct SensorInterfaceParameters
{
    uint16_t ThrottleMinPin0 = 500;
	uint16_t ThrottleMaxPin0 = 3500;
	uint16_t ThrottleMinPin1 = 500;
	uint16_t ThrottleMaxPin1 = 3500;
	uint16_t MaxTorque = 1660;

	uint16_t ThrottleSignalOutOfRangeThreshold = 20;
	uint16_t ThrottleSignalDeviationThreshold = MaxTorque / 10; // Maximum 10% deviation allowed. FSUK2024 T11.8.9
	uint16_t SignalDeadzone = 50;
};
const SensorInterfaceParameters sensorInterfaceParameters;

} // namespace VehicleControlUnit::MainLib::Settings


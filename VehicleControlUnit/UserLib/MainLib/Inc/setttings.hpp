# pragma once

namespace VehicleControlUnit { namespace MainLib { namespace Settings {

// Logger
const bool spamLoggingEnabled = false;
const bool infoLoggingEnabled = true;
const bool errorLoggingEnabled = true;

// MCU
const uint32_t implausibleThresholdInterval = 100;

// ADC
const uint32_t ADCDMABufferLength = 3;

// SensorInterface
const uint8_t throttleSignalADCIndex1 = 0;
const uint8_t throttleSignalADCIndex2 = 1;

}}} // namespace VehicleControlUnit::MainLib::Settings


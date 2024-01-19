# pragma once

namespace VehicleControlUnit { namespace MainLib { namespace Settings {

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
const uint32_t readyToDriveSoundDuration = 1500;

}}} // namespace VehicleControlUnit::MainLib::Settings


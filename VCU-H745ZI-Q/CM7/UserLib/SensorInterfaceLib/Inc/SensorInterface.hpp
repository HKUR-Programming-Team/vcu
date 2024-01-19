#pragma once

#include <UtilsLib/Inc/Logger.hpp>
#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ADCManager.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>

namespace VehicleControlUnit::SensorInterfaceLib {

class SensorInterface
{
public:
	// Constants
	const static uint16_t ThrottleMinPin0 = 1000;
	const static uint16_t ThrottleMaxPin0 = 64535;
	const static uint16_t ThrottleMinPin1 = 1000;
	const static uint16_t ThrottleMaxPin1 = 64535;
	const static uint16_t MaxTorque = 1660;

	const static uint16_t ThrottleSignalOutOfRangeThreshold = 300;
	const static uint16_t ThrottleSignalDeviationThreshold = MaxTorque / 10; // Maximum 10% deviation allowed. FSUK2024 T11.8.9
	const static uint16_t SignalDeadzone = 50;

	SensorInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, UtilsLib::ADCManager& ADCManager,
			const uint8_t throttleSignalADCIndex0,
			const uint8_t throttleSignalADCIndex1):
		mLogger{logger},
		mDataStore{dataStore},
		mADCManager{ADCManager},
		mThrottleSignalADCIndex0{throttleSignalADCIndex0},
		mThrottleSignalADCIndex1{throttleSignalADCIndex1}
	{}

	void ReadADC();

private:
	void ReadThrottleSignal();

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::ADCManager& mADCManager;

	const uint8_t mThrottleSignalADCIndex0;
	const uint8_t mThrottleSignalADCIndex1;
};

}

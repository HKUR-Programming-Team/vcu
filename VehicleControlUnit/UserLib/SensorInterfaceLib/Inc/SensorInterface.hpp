#pragma once

#include <UtilsLib/Inc/Logger.hpp>
#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ADCManager.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>

namespace VehicleControlUnit { namespace SensorInterfaceLib {

class SensorInterface
{
public:
	SensorInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, UtilsLib::ADCManager& ADCManager,
			const uint8_t throttleSignalADCIndex1,
			const uint8_t throttleSignalADCIndex2):
		mLogger{logger},
		mDataStore{dataStore},
		mADCManager{ADCManager},
		mThrottleSignalADCIndex1{throttleSignalADCIndex1},
		mThrottleSignalADCIndex2{throttleSignalADCIndex2}
	{}

	void ReadADC();

private:
	void ReadThrottleSignal();

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::ADCManager& mADCManager;

	const uint8_t mThrottleSignalADCIndex1;
	const uint8_t mThrottleSignalADCIndex2;
};

}}

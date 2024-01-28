#pragma once

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <MainLib/Inc/settings.hpp>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
	#include <UtilsLib/Inc/ADCManager.hpp>
	#include <UtilsLib/Inc/Logger.hpp>
#else
	#include <MockLibraries.hpp>
#endif

namespace VehicleControlUnit::SensorInterfaceLib {

class SensorInterface
{
public:

	SensorInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, UtilsLib::ADCManager& ADCManager,
			const uint8_t throttleSignalADCIndex0,
			const uint8_t throttleSignalADCIndex1,
			const MainLib::Settings::SensorInterfaceParameters& parameters):
		mLogger{logger},
		mDataStore{dataStore},
		mADCManager{ADCManager},
		mThrottleSignalADCIndex0{throttleSignalADCIndex0},
		mThrottleSignalADCIndex1{throttleSignalADCIndex1},
		mParameters{parameters}
	{}

	void ReadADC();
	UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);

	const MainLib::Settings::SensorInterfaceParameters mParameters;

private:
	void ReadThrottleSignal();

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::ADCManager& mADCManager;

	const uint8_t mThrottleSignalADCIndex0;
	const uint8_t mThrottleSignalADCIndex1;
};

}

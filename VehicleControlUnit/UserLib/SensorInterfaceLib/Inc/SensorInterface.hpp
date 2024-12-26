#pragma once

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <MainLib/Inc/Config.hpp>

#include <optional>

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
			const MainLib::Config::Config& config):
		mThrottleConfig{config.mSensorInterfaceThrottleConfig},
		mBrakeConfig{config.mSensorInterfaceBrakeConfig},
		mRegenConfig{config.mSensorInterfaceRegenConfig},
		mLogger{logger},
		mDataStore{dataStore},
		mADCManager{ADCManager}
	{}

	void ReadADC();
	UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);

	const MainLib::Config::SensorInterfaceThrottleConfig mThrottleConfig;
	const MainLib::Config::SensorInterfaceBrakeConfig mBrakeConfig;
	const MainLib::Config::SensorInterfaceRegenConfig mRegenConfig;

private:
	void ReadThrottleSignal();
	void ReadBrakeSignal();
	void ReadRegenSignal();

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::ADCManager& mADCManager;
};

}

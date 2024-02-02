#pragma once

#include <DataStoreLib/Inc/DataStore.hpp>
#include <MainLib/Inc/settings.hpp>
 
#ifndef MOCK_TEST
	#include <stm32f1xx.h>
	#include <UtilsLib/Inc/Logger.hpp>
	#include <UtilsLib/Inc/GPIOManager.hpp>
#else
	#include <MockLibraries.hpp>
#endif

namespace VehicleControlUnit::ReadyToDriveLib {

class ReadyToDrive
{
public:
	ReadyToDrive(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, const MainLib::Settings::ReadyToDriveParameters readyToDriveParameters):
			mLogger{logger},
			mDataStore{dataStore},
			mReadyToDriveMode{false},
			mReadyToDriveStartTs{0},
			mReadyToDriveButtonPreviousStateIsHigh{true},
			mParameters{readyToDriveParameters}
	{}

	void Check();
private:
	void CheckReadyToDriveMode(const bool currentlyReadyToDriveButtonIsHigh);
	void ControlReadyToDriveSound();
	void TurnOnSound();
	void TurnOffSound();
	void TurnOnReadyToDriveMode();
	void TurnOffReadyToDriveMode();

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;

	bool mReadyToDriveMode;
	uint32_t mReadyToDriveStartTs;
	bool mReadyToDriveButtonPreviousStateIsHigh;

	const MainLib::Settings::ReadyToDriveParameters mParameters;
};

};

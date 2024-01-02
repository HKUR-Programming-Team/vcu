#pragma once

#include <UtilsLib/Inc/Logger.hpp>
#include <DataStoreLib/Inc/DataStore.hpp>

namespace VehicleControlUnit { namespace ReadyToDriveLib {

class ReadyToDrive
{
public:
	ReadyToDrive(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, const uint32_t readyToDriveSoundDuration):
			mLogger{logger},
			mDataStore{dataStore},
			mReadyToDriveMode{false},
			mReadyToDriveStartTs{0},
			mReadyToDriveSoundDuration{readyToDriveSoundDuration}
	{}

	void CheckReadyToDrive();

private:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;

	bool mReadyToDriveMode;
	uint32_t mReadyToDriveStartTs;

	const uint32_t mReadyToDriveSoundDuration;
};

}};

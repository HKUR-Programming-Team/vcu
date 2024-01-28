#pragma once

#include <DataStoreLib/Inc/DriveDataStore.hpp>
#include <DataStoreLib/Inc/MCUAndBMSDataStore.hpp>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif

namespace VehicleControlUnit::DataStoreLib {

class DataStore
{
public:

	DataStore():
		mDriveDataStore(),
		mPersistedImplausibleStatus{false}
	{}

	bool GetPersistedImplausibleStatus() const
	{
		return mPersistedImplausibleStatus;
	}

	void SetPersistedImplausibleStatus(const bool status)
	{
		mPersistedImplausibleStatus = status;
	}

	DriveDataStore mDriveDataStore;
	MCUAndBMSDataStore mMCUAndBMSDataStore;

private:
	bool mPersistedImplausibleStatus;
};

} // namespace VehicleControlUnit::DataStore


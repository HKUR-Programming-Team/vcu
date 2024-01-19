#pragma once

#include <DataStoreLib/Inc/DriveDataStore.hpp>

#include <stm32h7xx.h>

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

private:
	bool mPersistedImplausibleStatus;
};

} // namespace VehicleControlUnit::DataStore


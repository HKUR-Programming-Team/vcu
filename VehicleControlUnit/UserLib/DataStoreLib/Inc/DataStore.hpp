#pragma once

#include <DataStoreLib/Inc/ThrottleDataStore.hpp>

#include <stm32f1xx.h>

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


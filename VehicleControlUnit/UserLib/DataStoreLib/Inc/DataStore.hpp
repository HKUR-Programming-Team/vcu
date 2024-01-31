#pragma once

#include <DataStoreLib/Inc/DrivingInputDataStore.hpp>
#include <DataStoreLib/Inc/MCUDataStore.hpp>
#include <DataStoreLib/Inc/VehicleSensorDataStore.hpp>

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
		mDrivingInputDataStore(),
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

	DrivingInputDataStore mDrivingInputDataStore;
	MCUDataStore mMCUDataStore;
	VehicleSensorDataStore mVehicleSensorDataStore;

private:
	bool mPersistedImplausibleStatus;
};

} // namespace VehicleControlUnit::DataStore


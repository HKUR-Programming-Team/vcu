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
		mPersistedImplausibleStatus{false},
		mBroadcastMessageReceiveTimeoutError{false},
		mCommandMessageFrequencyError(false)
	{}

	bool GetPersistedImplausibleStatus() const
	{
		return mPersistedImplausibleStatus;
	}

	bool GetBroadcastMessageReceiveTimeoutError() const 
    {
        return mBroadcastMessageReceiveTimeoutError;
    }

    bool GetCommandMessageFrequencyError() const
    {
        return mCommandMessageFrequencyError;
    }

	void SetPersistedImplausibleStatus(const bool status)
	{
		mPersistedImplausibleStatus = status;
	}

    void SetCommandMessageFrequencyError(const bool error)
    {
        mCommandMessageFrequencyError = error;
    }

	void SetBroadcastMessageReceiveTimeoutError(const bool error)
    {
        mBroadcastMessageReceiveTimeoutError = error;
    }

	DrivingInputDataStore mDrivingInputDataStore;
	MCUDataStore mMCUDataStore;
	VehicleSensorDataStore mVehicleSensorDataStore;

private:
	bool mPersistedImplausibleStatus;
	bool mBroadcastMessageReceiveTimeoutError;
    bool mCommandMessageFrequencyError;
};

} // namespace VehicleControlUnit::DataStore


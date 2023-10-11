#pragma once

#include <stm32f1xx.h>

namespace VehicleControlUnit { namespace DataStoreLib {

class DataStore
{
public:

	DataStore():
		mMCUImplausibleState{false},
		mBMSValidity{false},
		mBMSBatteryLevel{0},
		mThrottleValidity{false},
		mThrottleImplausibleState{false},
		mThrottle{0}
	{}

	// MCU
	bool GetMCUValidity() const
	{
		return mMCUValidity;
	}
	bool GetMCUImplausibleState() const
	{
		return mMCUImplausibleState;
	}

	void SetMCUValidity(const bool MCUValidity)
	{
		mMCUValidity = MCUValidity;
	}
	void SetMCUImplausibleState(const bool MCUImplausibleState)
	{
		mMCUImplausibleState = MCUImplausibleState;
	}

	// BMS
	bool GetBMSValidity() const
	{
		return mBMSValidity;
	}
	uint8_t GetBMSBatteryLevel() const
	{
		if (!mBMSValidity) return 0;

		return mBMSBatteryLevel;
	}

	void SetBMSValidity(const bool BMSValidity)
	{
		mBMSValidity = BMSValidity;
	}
	void SetBMSBatteryLevel(const uint8_t BMSBatteryLevel)
	{
		mBMSBatteryLevel = BMSBatteryLevel;
	}

	// Sensor: Throttle
	bool GetThrottleValidity() const
	{
		return mThrottleValidity;
	}
	bool GetThrottleImplausibleState() const
	{
		return mThrottleImplausibleState;
	}
	bool GetThrottle() const
	{
		if (!mThrottleValidity) return 0;

		return mThrottle;
	}

	void SetThrottleValidity(const bool throttleValidity)
	{
		mThrottleValidity = throttleValidity;
	}
	void SetThrottleImplausibleState(const bool MCUImplausibleState)
	{
		mMCUImplausibleState = MCUImplausibleState;
	}
	void SetThrottle(const bool throttle)
	{
		mThrottle = throttle;
	}


private:

// MCU
	bool mMCUValidity;
	bool mMCUImplausibleState;

// BMS
	bool mBMSValidity;
	uint8_t mBMSBatteryLevel;

// Sensor: Throttle
	bool mThrottleValidity;
	bool mThrottleImplausibleState;
	uint8_t mThrottle;

};

}} // namespace VehicleControlUnit::DataStore


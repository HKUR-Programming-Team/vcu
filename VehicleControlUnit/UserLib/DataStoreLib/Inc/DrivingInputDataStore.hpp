#pragma once

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif
namespace VehicleControlUnit::DataStoreLib {

enum class Gear
{
	FORWARD,
	NEUTRAL
};

class DrivingInputDataStore
{
public:
	DrivingInputDataStore():
			mThrottleError{false},
			mTorque{0},
			mGear{Gear::NEUTRAL}
	{}

	bool GetThrottleError() const
	{
		return mThrottleError;
	}

	void SetThrottleError(const bool error)
	{
		mThrottleError = error;
	}

	// Make sure to check that the throttle has no error before using the throttle value
	int16_t GetTorque() const
	{
		if (mThrottleError)
		{
			return 0;
		}

		return mTorque;
	}

	// Make sure to set the error status before using the throttle
	// Unit: torque in 0.1 [0,MaxTorque]
	void SetTorque(const int16_t throttle)
	{
		mTorque = throttle;
	}

	int16_t GetRegen() const
	{
		return mRegen;
	}

	void SetRegen(const int16_t& regen)
	{
		mRegen = regen;
	}

	Gear GetGear() const
	{
		return mGear;
	}

	void SetGear(const Gear& gear)
	{
		mGear = gear;
	}

private:
	bool mThrottleError;
	int16_t mTorque;
	int16_t mRegen;
	Gear mGear;
};

};

#pragma once

#include <stm32f1xx.h>

namespace VehicleControlUnit::DataStoreLib {

enum class Gear
{
	FORWARD,
	NEUTRAL
};

class DriveDataStore
{
public:
	DriveDataStore():
			mError{false},
			mTorque{0},
			mRegen{0},
			mGear{Gear::NEUTRAL}
	{}

	bool GetError() const
	{
		return mError;
	}

	void SetError(const bool error)
	{
		mError = error;
	}

	// Make sure to check that the throttle has no error before using the throttle value
	uint16_t GetTorque() const
	{
		if (mError)
		{
			return 0;
		}

		return mTorque;
	}

	// Make sure to set the error status before using the throttle
	// Unit: torque in 0.1 [0,MaxTorque]
	void SetTorque(const uint16_t throttle)
	{
		mTorque = throttle;
	}

	uint16_t GetRegen() const
	{
		return mRegen;
	}

	void SetRegen(const uint16_t regen)
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
	bool mError;
	uint16_t mTorque;
	uint16_t mRegen;
	Gear mGear;
};

};

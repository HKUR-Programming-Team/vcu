#pragma once

#include <optional>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif

namespace VehicleControlUnit::DataStoreLib {

class VehicleSensorDataStore
{
public:
	VehicleSensorDataStore():
		mLinearVelocity{std::nullopt},
		mLinearVelocityUpdateTs{0},
		mAngularWheelSpeedRearLeft{std::nullopt},
		mAngularWheelSpeedRearRight{std::nullopt},
		mAngularVelocityUpdateTs{0}
	{}

	void SetLinearVelocity(const std::optional<double>& linearVelocity)
	{
		mLinearVelocity = linearVelocity;
	}

	void SetLinearVelocityUpdateTs(const uint32_t ts)
	{
		mLinearVelocityUpdateTs = ts;
	}

	void SetAngularWheelSpeedRearLeft(const std::optional<double>& angularWheelSpeedRearLeft)
	{
		mAngularWheelSpeedRearLeft = angularWheelSpeedRearLeft;
	}

	void SetAngularWheelSpeedRearRight(const std::optional<double>& angularWheelSpeedRearRight)
	{
		mAngularWheelSpeedRearRight = angularWheelSpeedRearRight;
	}

	void SetAngularVelocityUpdateTs(const uint32_t ts)
	{
		mAngularVelocityUpdateTs = ts;
	}

	std::optional<double> GetLinearVelocity() const
	{
		return mLinearVelocity;
	}

	uint32_t GetLinearVelocityUpdateTs(const uint32_t ts)
	{
		return mLinearVelocityUpdateTs;
	}

	std::optional<double> GetAngularWheelSpeedRearLeft() const
	{
		return mAngularWheelSpeedRearLeft;
	}

	std::optional<double> GetAngularWheelSpeedRearRight() const
	{
		return mAngularWheelSpeedRearRight;
	}

	uint32_t GetAngularVelocityUpdateTs(const uint32_t ts)
	{
		return mAngularVelocityUpdateTs;
	}

private:
	std::optional<double> mLinearVelocity;
	uint32_t mLinearVelocityUpdateTs;

	std::optional<double> mAngularWheelSpeedRearLeft;
	std::optional<double> mAngularWheelSpeedRearRight;
	uint32_t mAngularVelocityUpdateTs;
};

};

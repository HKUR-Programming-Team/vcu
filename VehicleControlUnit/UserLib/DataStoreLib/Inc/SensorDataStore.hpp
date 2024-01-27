#pragma once

#include <optional>

#include <stm32f1xx.h>

namespace VehicleControlUnit::DataStoreLib {

class SensorDataStore
{
public:
	void SetLinearVelocity(const std::optional<uint16_t>& linearVelocity)
	{

	}

	void SetAngularWheelSpeedRearLeft(const std::optional<uint16_t>& angularWheelSpeedRearLeft)
	{

	}

	void SetAngularWheelSpeedRearRight(const std::optional<uint16_t>& angularWheelSpeedRearRight)
	{

	}

	std::optional<uint16_t> GetLinearVelocity() const
	{
		return mLinearVelocity;
	}

	std::optional<uint16_t> GetAngularWheelSpeedRearLeft() const
	{
		return mAngularWheelSpeedRearLeft;
	}

	std::optional<uint16_t> GetAngularWheelSpeedRearRight() const
	{
		return mAngularWheelSpeedRearRight;
	}

private:
	std::optional<uint16_t> mLinearVelocity;
	std::optional<uint16_t> mAngularWheelSpeedRearLeft;
	std::optional<uint16_t> mAngularWheelSpeedRearRight;

};

};

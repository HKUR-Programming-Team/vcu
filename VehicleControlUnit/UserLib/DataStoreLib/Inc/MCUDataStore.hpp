#pragma once

#include <optional>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif
namespace VehicleControlUnit::DataStoreLib {

class MCUDataStore
{
public:
	MCUDataStore():
			mMotorSpeed{std::nullopt},
            mMotorSpeedUpdateTs{std::nullopt},
			mCommandMessageFrequency{std::nullopt},
            mMessageReceiveTimeoutError{false}
	{}

    void SetMotorSpeed(const std::optional<int16_t> motorSpeed)
    {
        mMotorSpeed = motorSpeed;
    }

    void SetMotorSpeedUpdateTs(const std::optional<uint32_t> ts)
    {
        mMotorSpeedUpdateTs = ts;
    }

    void SetCommandMessageFrequency(const std::optional<uint32_t> freq)
    {
    	mCommandMessageFrequency = freq;
    }

    void SetMessageReceiveTimeoutError(const bool error)
    {
        mMessageReceiveTimeoutError = error;
    }

    // actual velocity (in RPM)
    std::optional<int16_t> GetMotorSpeed() const
    {
        return mMotorSpeed;
    }

    std::optional<uint32_t> GetMotorSpeedUpdateTs() const
    {
        return mMotorSpeedUpdateTs;
    }

    std::optional<uint32_t> GetCommandMessageFrequency() const
    {
    	return mCommandMessageFrequency;
    }

    bool GetMessageReceiveTimeoutError() const 
    {
        return mMessageReceiveTimeoutError;
    }

private:
    std::optional<int16_t> mMotorSpeed;
    std::optional<uint32_t> mMotorSpeedUpdateTs;
    std::optional<uint32_t> mCommandMessageFrequency;
    bool mMessageReceiveTimeoutError;
};

};

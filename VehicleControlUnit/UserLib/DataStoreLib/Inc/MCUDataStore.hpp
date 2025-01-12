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
            mLastMCUBroadcastMessageReceiveTs{std::nullopt},
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

    void SetLastMCUBroadcastMessageReceiveTs(const std::optional<uint32_t> ts)
    {
        mLastMCUBroadcastMessageReceiveTs = ts;
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

    std::optional<uint32_t> GetLastMCUBroadcastMessageReceiveTs() const
    {
        return mLastMCUBroadcastMessageReceiveTs;
    }

private:
    std::optional<int16_t> mMotorSpeed;
    std::optional<uint32_t> mMotorSpeedUpdateTs;
    std::optional<uint32_t> mCommandMessageFrequency;
    std::optional<uint32_t> mLastMCUBroadcastMessageReceiveTs;
    bool mMessageReceiveTimeoutError;
};

};

#pragma once

#include <optional>

#ifndef MOCK_TEST
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif
namespace VehicleControlUnit::DataStoreLib {

enum class VSMState 
{
    StartUp,
    PreChargeInit,
    PreChargeActivate,
    PreChangeComplete,
    Idle,
    Ready,
    MotorRunning,
    BlinkFaultCode,
    ShutdownInProgress,
    PowerRecycling,
    Unknown
};

class MCUDataStore
{
public:
	MCUDataStore():
			mMotorSpeed{0},
			mDCBusCurrent{0},
			mDCBusVoltage{0},
			mVSMState{VSMState::Unknown},
            mUpdateTs{0}
	{}

    void SetMotorSpeed(const int16_t motorSpeed)
    {
        mMotorSpeed = motorSpeed;
    }

    void SetDCBusCurrent(const int16_t dcBusCurrent)
    {
        mDCBusCurrent = dcBusCurrent;
    }

    void SetDCBusVoltage(const int16_t dcBusVoltage)
    {
        mDCBusVoltage = dcBusVoltage;
    }

    void SetVSMState(const VSMState vsmState)
    {
        mVSMState = vsmState;
    }

    void SetUpdateTs(const uint32_t ts)
    {
        mUpdateTs = ts;
    }

    // actual velocity (in RPM)
    int16_t GetMotorSpeed() const
    {
        return mMotorSpeed;
    }

    // actual voltage (in Volts) times 10
    int16_t GetDCBusCurrent() const
    {
        return mDCBusCurrent;
    }

    // actual current (in Amps) times 10
    int16_t GetDCBusVoltage() const
    {
        return mDCBusVoltage;
    }

    VSMState GetVSMState() const
    {
        return mVSMState;
    }

    uint16_t GetUpdateTs() const
    {
        return mUpdateTs;
    }

private:
    int16_t mMotorSpeed;
    int16_t mDCBusCurrent;
    int16_t mDCBusVoltage;
    VSMState mVSMState;
    uint32_t mUpdateTs;
};

};

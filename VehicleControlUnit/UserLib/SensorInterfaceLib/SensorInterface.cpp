#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

namespace VehicleControlUnit::SensorInterfaceLib {

void SensorInterface::ReadADC()
{
	ReadThrottleSignal();
	ReadBrakeSignal();
	ReadRegenSignal();
}

void SensorInterface::ReadThrottleSignal()
{
	uint16_t reading0 = 0;
	uint16_t reading1 = 0;

	mADCManager.GetBufferByIndex(mThrottleConfig.ThrottleSignalADCIndex1, reading0);
	mADCManager.GetBufferByIndex(mThrottleConfig.ThrottleSignalADCIndex2, reading1);

	mLogger.LogCustom("Throttle: " + std::to_string(reading0) + ", " + std::to_string(reading1));

	// Check if the raw value deviates from max/min value of the throttle too much. FSUK2024 T11.9.2(c)
	const bool pin0OutOfBottomRange = mThrottleConfig.ThrottleMinPin0 > mThrottleConfig.ThrottleSignalOutOfRangeThreshold 
			&& reading0 < mThrottleConfig.ThrottleMinPin0 - mThrottleConfig.ThrottleSignalOutOfRangeThreshold;
	const bool pin0OutOfTopRange = reading0 > mThrottleConfig.ThrottleMaxPin0 + mThrottleConfig.ThrottleSignalOutOfRangeThreshold;
	if (pin0OutOfBottomRange || pin0OutOfTopRange)
	{
		mDataStore.mDrivingInputDataStore.SetThrottleError(true);
		mLogger.LogSpam("Throttle Pin 0 giving impossible value. min = " + std::to_string(mThrottleConfig.ThrottleMinPin0) + ", max = " + std::to_string(mThrottleConfig.ThrottleMaxPin0) + ", reading = " + std::to_string(reading0));
		return;
	}

	const bool pin1OutOfBottomRange = mThrottleConfig.ThrottleMinPin1 > mThrottleConfig.ThrottleSignalOutOfRangeThreshold 
			&& reading1 < mThrottleConfig.ThrottleMinPin1 - mThrottleConfig.ThrottleSignalOutOfRangeThreshold;
	const bool pin1OutOfTopRange = reading1 > mThrottleConfig.ThrottleMaxPin1 + mThrottleConfig.ThrottleSignalOutOfRangeThreshold;
	if (pin1OutOfBottomRange || pin1OutOfTopRange)
	{
		mDataStore.mDrivingInputDataStore.SetThrottleError(true);
		mLogger.LogSpam("Throttle Pin 1 giving impossible value. min = " + std::to_string(mThrottleConfig.ThrottleMinPin1) + ", max = " + std::to_string(mThrottleConfig.ThrottleMaxPin1) + ", reading = " + std::to_string(reading1));
		return;
	}

	auto mapThrottle = [&](const uint16_t readingMin, const uint16_t readingMax, const uint16_t reading) -> int16_t
	{
		// Slightly out of range signal due to noises. Deadzone of throttle signal
		if (reading < readingMin + mThrottleConfig.SignalDeadzone)
		{
			return 0;
		}
		if (reading > readingMax - mThrottleConfig.SignalDeadzone)
		{
			return static_cast<int16_t>(mThrottleConfig.MaxTorque);
		}

		return static_cast<int16_t>((reading - readingMin) * mThrottleConfig.MaxTorque / (readingMax - readingMin));
	};

	const int16_t throttle0 = mapThrottle(mThrottleConfig.ThrottleMinPin0, mThrottleConfig.ThrottleMaxPin0, reading0);
	const int16_t throttle1 = mapThrottle(mThrottleConfig.ThrottleMinPin1, mThrottleConfig.ThrottleMaxPin1, reading1);

	if (throttle0 > throttle1 && throttle0 - throttle1 > mThrottleConfig.ThrottleSignalDeviationThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetThrottleError(true);
		mLogger.LogSpam("Throttle signal deviation more than 10%");
		return;
	}
	if (throttle1 > throttle0 && throttle1 - throttle0 > mThrottleConfig.ThrottleSignalDeviationThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetThrottleError(true);
		mLogger.LogSpam("Throttle signal deviation more than 10%");
		return;
	}

	mLogger.LogSpam("Throttle: " + std::to_string(throttle0) + ", " + std::to_string(throttle1));

	mDataStore.mDrivingInputDataStore.SetThrottleError(false);
	mDataStore.mDrivingInputDataStore.SetTorque(throttle0);
	mLogger.LogSpam("Throttle Final: " + std::to_string(throttle0));

	return;
}

void SensorInterface::ReadBrakeSignal()
{
	uint16_t reading = 0;
	mADCManager.GetBufferByIndex(mBrakeConfig.BrakeSignalADCIndex, reading);

	mLogger.LogCustom("Brake: " + std::to_string(reading));

	if (reading > mBrakeConfig.BrakeMaxPin + mBrakeConfig.BrakeSignalOutOfRangeThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetBrake(std::nullopt);
		return;
	}

	if (mBrakeConfig.BrakeMinPin > mBrakeConfig.BrakeSignalOutOfRangeThreshold 
			&& reading < mBrakeConfig.BrakeMinPin - mBrakeConfig.BrakeSignalOutOfRangeThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetBrake(std::nullopt);
		return;
	}

	if (reading > mBrakeConfig.BrakeMaxPin)
	{
		mDataStore.mDrivingInputDataStore.SetBrake(mBrakeConfig.MaxBrake);
		return;
	}

	if (reading < mBrakeConfig.BrakeMinPin)
	{
		mDataStore.mDrivingInputDataStore.SetBrake(0);
		return;
	}

	const auto value = (reading - mBrakeConfig.BrakeMinPin) * mBrakeConfig.MaxBrake / (mBrakeConfig.BrakeMaxPin - mBrakeConfig.BrakeMinPin);
	mDataStore.mDrivingInputDataStore.SetBrake(value);
}

void SensorInterface::ReadRegenSignal()
{
	uint16_t reading = 0;
	mADCManager.GetBufferByIndex(mRegenConfig.RegenSignalADCIndex, reading);

	mLogger.LogCustom("Regen: " + std::to_string(reading));

	if (reading > mRegenConfig.RegenMaxPin)
	{
		mDataStore.mDrivingInputDataStore.SetRegen(mRegenConfig.MaxRegen);
		return;
	}

	if (reading < mRegenConfig.RegenMinPin)
	{
		mDataStore.mDrivingInputDataStore.SetRegen(0);
		return;
	}

	const auto value = static_cast<int16_t>((reading - mRegenConfig.RegenMinPin) * mRegenConfig.MaxRegen / (mRegenConfig.RegenMaxPin - mRegenConfig.RegenMinPin));
	mDataStore.mDrivingInputDataStore.SetRegen(value);
}

UtilsLib::ErrorState SensorInterface::MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Handle the CAN message received from Sensors, store the data in the DataStore");
	mLogger.LogSpam("SensorCAN receives: " + std::to_string(header.StdId) + " " + std::to_string(message[0]) + ", " +
					std::to_string(message[1]));
	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

}

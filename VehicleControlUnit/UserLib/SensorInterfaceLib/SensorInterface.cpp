#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

namespace VehicleControlUnit::SensorInterfaceLib {

void SensorInterface::ReadADC()
{
	ReadThrottleSignal();
}

void SensorInterface::ReadThrottleSignal()
{
	uint16_t reading0 = 0;
	uint16_t reading1 = 0;

	mADCManager.GetBufferByIndex(mThrottleSignalADCIndex0, reading0);
	mADCManager.GetBufferByIndex(mThrottleSignalADCIndex1, reading1);

	mLogger.LogCustom("Read signals: " + std::to_string(reading0) + ", " + std::to_string(reading1));

	// Check if the raw value deviates from max/min value of the throttle too much. FSUK2024 T11.9.2(c)
	if (reading0 < mParameters.ThrottleMinPin0 - mParameters.ThrottleSignalOutOfRangeThreshold || reading0 > mParameters.ThrottleMaxPin0 + mParameters.ThrottleSignalOutOfRangeThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetError(true);
		mLogger.LogError("Throttle Pin 0 giving impossible value. min = " + std::to_string(mParameters.ThrottleMinPin0) + ", max = " + std::to_string(mParameters.ThrottleMaxPin0) + ", reading = " + std::to_string(reading0));
		return;
	}
	if (reading1 < mParameters.ThrottleMinPin1 - mParameters.ThrottleSignalOutOfRangeThreshold || reading1 > mParameters.ThrottleMaxPin1 + mParameters.ThrottleSignalOutOfRangeThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetError(true);
		mLogger.LogError("Throttle Pin 1 giving impossible value. min = " + std::to_string(mParameters.ThrottleMinPin1) + ", max = " + std::to_string(mParameters.ThrottleMaxPin1) + ", reading = " + std::to_string(reading1));
		return;
	}

	auto mapThrottle = [&](const uint16_t readingMin, const uint16_t readingMax, const uint16_t reading) -> int16_t
	{
		// Slightly out of range signal due to noises. Deadzone of throttle signal
		if (reading < readingMin + mParameters.SignalDeadzone)
		{
			return 0;
		}
		if (reading > readingMax - mParameters.SignalDeadzone)
		{
			return static_cast<int16_t>(mParameters.MaxTorque);
		}

		return static_cast<int16_t>((reading - readingMin) * mParameters.MaxTorque / (readingMax - readingMin));
	};

	const int16_t throttle0 = mapThrottle(mParameters.ThrottleMinPin0, mParameters.ThrottleMaxPin0, reading0);
	const int16_t throttle1 = mapThrottle(mParameters.ThrottleMinPin1, mParameters.ThrottleMaxPin1, reading1);

	if (throttle0 > throttle1 && throttle0 - throttle1 > mParameters.ThrottleSignalDeviationThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetError(true);
		mLogger.LogError("Throttle signal deviation more than 10%");
		return;
	}
	if (throttle1 > throttle0 && throttle1 - throttle0 > mParameters.ThrottleSignalDeviationThreshold)
	{
		mDataStore.mDrivingInputDataStore.SetError(true);
		mLogger.LogError("Throttle signal deviation more than 10%");
		return;
	}

	mLogger.LogSpam("Throttle: " + std::to_string(throttle0) + ", " + std::to_string(throttle1));

	mDataStore.mDrivingInputDataStore.SetError(false);
	mDataStore.mDrivingInputDataStore.SetTorque(throttle0);
	mLogger.LogCustom("Throttle Final: " + std::to_string(throttle0));

	return;
}

UtilsLib::ErrorState SensorInterface::MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Handle the CAN message received from Sensors, store the data in the DataStore");
	mLogger.LogSpam("SensorCAN receives: " + std::to_string(header.StdId) + " " + std::to_string(message[0]) + ", " +
					std::to_string(message[1]));
	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

}

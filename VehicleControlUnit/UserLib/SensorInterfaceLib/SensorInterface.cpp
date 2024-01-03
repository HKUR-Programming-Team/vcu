#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

namespace VehicleControlUnit { namespace SensorInterfaceLib {

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

	mLogger.LogSpam("Read signals: " + std::to_string(reading0) + ", " + std::to_string(reading1));

	// Check if the raw value deviates from max/min value of the throttle too much. FSUK2024 T11.9.2(c)
	if (reading0 < ThrottleMinPin0 - ThrottleSignalOutOfRangeThreshold || reading0 > ThrottleMaxPin0 + ThrottleSignalOutOfRangeThreshold)
	{
		mDataStore.mDriveDataStore.SetError(true);
		mLogger.LogError("Throttle Pin 0 giving impossible value. min = " + std::to_string(ThrottleMinPin0) + ", max = " + std::to_string(ThrottleMaxPin0) + ", reading = " + std::to_string(reading0));
		return;
	}
	if (reading1 < ThrottleMinPin1 - ThrottleSignalOutOfRangeThreshold || reading1 > ThrottleMaxPin1 + ThrottleSignalOutOfRangeThreshold)
	{
		mDataStore.mDriveDataStore.SetError(true);
		mLogger.LogError("Throttle Pin 1 giving impossible value. min = " + std::to_string(ThrottleMinPin1) + ", max = " + std::to_string(ThrottleMaxPin1) + ", reading = " + std::to_string(reading1));
		return;
	}

	auto mapThrottle = [&](const uint16_t readingMin, const uint16_t readingMax, const uint16_t reading) -> uint16_t
	{
		// Slightly out of range signal due to noises. Deadzone of throttle signal
		if (reading < readingMin + SignalDeadzone)
		{
			return 0;
		}
		if (reading > readingMax - SignalDeadzone)
		{
			return MaxTorque;
		}

		return (reading - readingMin) * MaxTorque / (readingMax - readingMin);
	};

	const auto throttle0 = mapThrottle(ThrottleMinPin0, ThrottleMaxPin0, reading0);
	const auto throttle1 = mapThrottle(ThrottleMinPin1, ThrottleMaxPin1, reading1);

	if (throttle0 > throttle1 && throttle0 - throttle1 > ThrottleSignalDeviationThreshold)
	{
		mDataStore.mDriveDataStore.SetError(true);
		mLogger.LogError("Throttle signal deviation more than 10%");
		return;
	}
	if (throttle1 > throttle0 && throttle1 - throttle0 > ThrottleSignalDeviationThreshold)
	{
		mDataStore.mDriveDataStore.SetError(true);
		mLogger.LogError("Throttle signal deviation more than 10%");
		return;
	}

	mLogger.LogSpam("Throttle: " + std::to_string(throttle0) + ", " + std::to_string(throttle1));

	mDataStore.mDriveDataStore.SetError(false);
	mDataStore.mDriveDataStore.SetThrottleSignal(throttle0);
	mLogger.LogSpam("Throttle Final: " + std::to_string(throttle0));

	return;
}

}}

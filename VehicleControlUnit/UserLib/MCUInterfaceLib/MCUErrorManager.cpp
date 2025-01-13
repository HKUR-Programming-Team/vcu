#include <MCUInterfaceLib/Inc/MCUErrorManager.hpp>

namespace VehicleControlUnit::MCUInterfaceLib {

void MCUErrorManager::CheckImplausibility()
{
	if (mDataStore.GetPersistedImplausibleStatus())
	{
		return;
	}

	const auto implausible = IsCurrentlyImplausible();

	if (!implausible)
	{
		mInImplausibleState = false;
		return;
	}

	if (!mInImplausibleState)
	{
		mInImplausibleState = true;
		mImplausibleStartTs = HAL_GetTick();
		return;
	}

	if (HAL_GetTick() - mImplausibleStartTs > mImplausibleThresholdInterval)
	{
		mLogger.LogError("There is Persisted Implausibility!");
		mDataStore.SetPersistedImplausibleStatus(true);
		return;
	}
}

void MCUErrorManager::CheckMCUTimeout()
{
	const auto lastMessageTsOpt = mDataStore.mMCUDataStore.GetLastMCUBroadcastMessageReceiveTs();
	if (!lastMessageTsOpt.has_value())
	{
		return; // First message from MCU not yet received
	}

	const auto currentTime = HAL_GetTick();

	if(!mFirstMCUBroadcastMessageReceivedTs.has_value())
	{
		mLogger.LogInfo("First MCU broadcast message received");
		mFirstMCUBroadcastMessageReceivedTs = currentTime;
	}

	const auto lastMessageTs = lastMessageTsOpt.value();
	if (currentTime > lastMessageTs + 500)
	{
		mDataStore.SetBroadcastMessageReceiveTimeoutError(true);
	}
}

void MCUErrorManager::CheckCommandMessageFrequency()
{
	const auto lastMessageTsOpt = mDataStore.mMCUDataStore.GetLastMCUBroadcastMessageReceiveTs();
	if (!lastMessageTsOpt.has_value())
	{
		return; // First message from MCU not yet received
	}

	const auto currentTime = HAL_GetTick();

	if(!mFirstMCUBroadcastMessageReceivedTs.has_value())
	{
		mLogger.LogInfo("First MCU broadcast message received");
		mFirstMCUBroadcastMessageReceivedTs = lastMessageTsOpt.value();
		return; // Wait for 1 second after the first broadcast message is received before starting to monitor
	}
	else if(currentTime < mFirstMCUBroadcastMessageReceivedTs.value() + 1000)
	{
		return; // Wait for 1 second after the first broadcast message is received before starting to monitor
	}

	const auto commandMessageTransmitFrequencyOpt = mDataStore.mMCUDataStore.GetCommandMessageFrequency();
	if (!commandMessageTransmitFrequencyOpt.has_value())
	{
		return;
	}

	const auto frequency = commandMessageTransmitFrequencyOpt.value();
	if (frequency < 20)
	{
		mDataStore.SetCommandMessageFrequencyError(true);
	}
}

void MCUErrorManager::ResetErrorState()
{
	mDataStore.SetPersistedImplausibleStatus(false);
	mDataStore.SetCommandMessageFrequencyError(false);
	mDataStore.SetBroadcastMessageReceiveTimeoutError(false);
	mInImplausibleState = false;
}

bool MCUErrorManager::IsCurrentlyImplausible() const
{
	const auto implausible = mDataStore.mDrivingInputDataStore.GetThrottleError() || mDataStore.mMCUDataStore.GetMessageReceiveTimeoutError();
	return implausible;
}

}

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

void MCUErrorManager::ResetErrorState()
{
	mDataStore.SetPersistedImplausibleStatus(false);
	mInImplausibleState = false;
}

bool MCUErrorManager::IsCurrentlyImplausible() const
{
	const auto implausible = mDataStore.mDrivingInputDataStore.GetThrottleError() || mDataStore.mMCUDataStore.GetMessageReceiveTimeoutError();
	return implausible;
}

}

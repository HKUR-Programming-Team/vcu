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
	mLogger.LogInfo("TODO: MCUErrorManager check if currently implausible ");
	const auto implausible = mDataStore.mDrivingInputDataStore.GetError(); // More conditions to be added?
	return implausible;
}

}

#include <DashboardInterfaceLib/Inc/DashboardInterface.hpp>

namespace VehicleControlUnit::DashboardInterfaceLib {

void DashboardInterface::DisplayDashboard()
{
	DisplayError();
}

void DashboardInterface::DisplayError()
{
	const auto persistedImplausibility = mDataStore.GetPersistedImplausibleStatus();
	UtilsLib::GPIOManager::digitalWrite(mParameters.ImplausibleErrorLightPort, mParameters.ImplausibleErrorLightPinNum, persistedImplausibility);

	const auto throttleSignalError = mDataStore.mDrivingInputDataStore.GetThrottleError();
	UtilsLib::GPIOManager::digitalWrite(mParameters.ThrottleSignalErrorLightPort, mParameters.ThrottleSignalErrorLightPinNum, throttleSignalError);

	const auto brakeSignalError = !mDataStore.mDrivingInputDataStore.GetBrake().has_value();
	UtilsLib::GPIOManager::digitalWrite(mParameters.BrakeSignalErrorLightPort, mParameters.BrakeSignalErrorLightPinNum, brakeSignalError);

	const auto mcuTimeoutError = mDataStore.mMCUDataStore.GetMessageReceiveTimeoutError();
	UtilsLib::GPIOManager::digitalWrite(mParameters.MCUTimeoutErrorLightPort, mParameters.MCUTimeoutErrorLightPinNum, mcuTimeoutError);
}

};

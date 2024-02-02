#include <ReadyToDriveLib/Inc/ReadyToDrive.hpp>

namespace VehicleControlUnit::ReadyToDriveLib {

void ReadyToDrive::Check()
{
	const auto currentlyReadyToDriveButtonIsHigh = UtilsLib::GPIOManager::digitalRead(mParameters.readyToDriveButtonPort, mParameters.readyToDriveButtonPinNum);
	CheckReadyToDriveMode(currentlyReadyToDriveButtonIsHigh);
	ControlReadyToDriveSound();
	mReadyToDriveButtonPreviousStateIsHigh = currentlyReadyToDriveButtonIsHigh;
}

void ReadyToDrive::CheckReadyToDriveMode(const bool currentlyReadyToDriveButtonIsHigh)
{
	const auto readyToDriveButtonPressed = mReadyToDriveButtonPreviousStateIsHigh && !currentlyReadyToDriveButtonIsHigh;

	if (!readyToDriveButtonPressed)
	{
		return;
	}

	const auto brakeValueAvailable = mDataStore.mDrivingInputDataStore.GetBrake().has_value();

	if (!brakeValueAvailable)
	{
		TurnOffReadyToDriveMode();
		return;
	}

	const auto brakeValue = mDataStore.mDrivingInputDataStore.GetBrake().value();
	if (brakeValue < mParameters.readyToDriveTriggeringBrakeThreshold)
	{
		TurnOffReadyToDriveMode();
		return;
	}

	TurnOnReadyToDriveMode();
}

void ReadyToDrive::ControlReadyToDriveSound()
{
	if (!mReadyToDriveMode)
	{
		TurnOffSound();
		return;
	}

	if (HAL_GetTick() - mReadyToDriveStartTs >= mParameters.readyToDriveSoundDuration)
	{
		TurnOffSound();
		return;
	}

	TurnOnSound();
}

void ReadyToDrive::TurnOnSound()
{
	UtilsLib::GPIOManager::digitalWrite(mParameters.readyToDriveSoundPort, mParameters.readyToDriveSoundPinNum, true);
}

void ReadyToDrive::TurnOffSound()
{
	UtilsLib::GPIOManager::digitalWrite(mParameters.readyToDriveSoundPort, mParameters.readyToDriveSoundPinNum, false);
}

void ReadyToDrive::TurnOnReadyToDriveMode()
{
	mReadyToDriveMode = true;
	mDataStore.mDrivingInputDataStore.SetGear(DataStoreLib::Gear::FORWARD);
	mReadyToDriveStartTs = HAL_GetTick();
}

void ReadyToDrive::TurnOffReadyToDriveMode()
{
	mReadyToDriveMode = false;
	mDataStore.mDrivingInputDataStore.SetGear(DataStoreLib::Gear::NEUTRAL);
}

};

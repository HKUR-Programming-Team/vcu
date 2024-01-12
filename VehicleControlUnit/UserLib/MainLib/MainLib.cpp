#include <MainLib/Inc/MainLib.hpp>

namespace VehicleControlUnit { namespace MainLib {

void Main::Setup()
{
	mLogger.LogInfo("--VCU Setup Starts--");
	const auto canError = mCanManagerForBMSAndMCU.init();
	const auto adcError = mADCManager.init();

	if (canError != UtilsLib::ErrorState::INIT_SUCCESS || adcError != UtilsLib::ErrorState::INIT_SUCCESS)
	{
		mSetupFailed = true;
		mLogger.LogInfo("--VCU Setup Failed-- (TODO: Better error message");
		return;
	}

	mSetupFailed = false;
	mLogger.LogInfo("--VCU Setup Finish--");
}

void Main::Loop()
{
	if (mSetupFailed)
	{
		mLogger.LogInfo("TODO: what has to be done if in error");
		return;
	}

	mLogger.LogSpam("--VCU Loop Starts--");

	// Sensor
	mSensorInterface.ReadADC(); // Read throttle and other analog signals and store it to dataStore

	// MCU
	mMCUErrorManager.CheckImplausibility(); // update implausible status
	mLogger.LogCustom("Persisted: " + std::to_string(mDataStore.GetPersistedImplausibleStatus()));

	mMCUInterface.SendMessage(); // send packet to motor controller

	// Ready to Drive
	mReadyToDriveManager.CheckReadyToDrive();
}

void Main::CANMessageReceiveHandlerFIFO0(const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	// Copy the message so that the message does not get overwritten
	// TODO :: determine whether CANMessageReceiveHandlerFIFO0 message copy is necessary
	mLogger.LogInfo("TODO: is CANMessageReceiveHandlerFIFO0 message copy necessary?");
	uint8_t messageBuffer[8];
	for (int i = 0; i < 8; i++)
	{
		messageBuffer[i] = message[i];
	}

	mCanManagerForBMSAndMCU.MessageReceiveHandler(header, messageBuffer);
}

}}

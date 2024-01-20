#include <MainLib/Inc/MainLib.hpp>

namespace VehicleControlUnit::MainLib {

void Main::Setup()
{
	mLogger.LogInfo("--VCU Setup Starts--");
	const auto canErrorBMSAndMCU = mCanManagerForBMSAndMCU.init();
	const auto canErrorSensors = mCanManagerForSensors.init();
	const auto adcError = mADCManager.init();

	if (canErrorBMSAndMCU != UtilsLib::ErrorState::INIT_SUCCESS ||
			canErrorSensors != UtilsLib::ErrorState::INIT_SUCCESS ||
			adcError != UtilsLib::ErrorState::INIT_SUCCESS)
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

	uint8_t txArr[8] = {1,3,5,6,9,12,14,15};
//	mCanManagerForSensors.SetTransmitHeader(0x10, false);
//	mCanManagerForSensors.SendMessage(txArr);

	uint8_t txArr2[8] = {2,4,5,6,9,12,14,16};
//	mCanManagerForBMSAndMCU.SetTransmitHeader(0x10, false);
//	mCanManagerForBMSAndMCU.SendMessage(txArr2);

	mCanManagerForSensors.CheckReceiveFIFO();
	mCanManagerForBMSAndMCU.CheckReceiveFIFO();

//	// MCU
//	mMCUErrorManager.CheckImplausibility(); // update implausible status
//	mLogger.LogCustom("Persisted: " + std::to_string(mDataStore.GetPersistedImplausibleStatus()));
//
//	mMCUInterface.SendMessage(); // send packet to motor controller
//
//	// Ready to Drive
//	mReadyToDriveManager.CheckReadyToDrive();
}

}

#include <MainLib/Inc/MainLib.hpp>

namespace VehicleControlUnit::MainLib {

void Main::Setup()
{
	mLogger.LogInfo("--VCU Setup Starts--");
	const auto canError = mCANManager.init();
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

	UtilsLib::GPIOManager::digitalRead(UtilsLib::GPIOPort::A, UtilsLib::GPIOPinNum::Pin1);

	mLogger.LogSpam("--VCU Loop Starts--");
//	uint8_t test[8] = {1,2,3,4,5,6,7,8};
//	mCANManager.SetTransmitHeader(0x0C0, 8, false);
//	mCANManager.SendMessage(test);
	mCANManager.CheckReceiveFIFO();

	// Sensor
	mSensorInterface.ReadADC(); // Read throttle and other analog signals and store it to dataStore

	// MCU
	mMCUErrorManager.CheckImplausibility(); // update implausible status
	mLogger.LogSpam("Persisted: " + std::to_string(mDataStore.GetPersistedImplausibleStatus()));

	mMCUInterface.SendMessage(); // send packet to motor controller

	// Ready to Drive
	mReadyToDriveManager.CheckReadyToDrive();
}

}

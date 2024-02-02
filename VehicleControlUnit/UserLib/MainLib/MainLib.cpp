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
		mMCUInterface.SendCommandMessageInErrorState();
		return;
	}

	// TEST CODE STARTS
	//	bool button = UtilsLib::GPIOManager::digitalRead(UtilsLib::GPIOPort::C, UtilsLib::GPIOPinNum::Pin7);
	//	UtilsLib::GPIOManager::digitalWrite(UtilsLib::GPIOPort::A, UtilsLib::GPIOPinNum::Pin9,button);
	//	uint8_t test[8] = {1,2,3,4,5,6,7,8};
	//	mCANManager.SetTransmitHeader(0x0C0, 8, false);
	//	mCANManager.SendMessage(test);
	// TEST CODE ENDS

	mLogger.LogSpam("--VCU Loop Starts--");
	mCANManager.CheckReceiveFIFO();

	// Ready to Drive
	mReadyToDriveManager.Check();
	mDataStore.mDrivingInputDataStore.SetGear(DataStoreLib::Gear::FORWARD);

	// Sensor
	mSensorInterface.ReadADC(); // Read throttle and other analog signals and store it to dataStore

	// MCU
	mMCUErrorManager.CheckImplausibility(); // update implausible status

	mMCUInterface.SendCommandMessage(); // send packet to motor controller
}

}

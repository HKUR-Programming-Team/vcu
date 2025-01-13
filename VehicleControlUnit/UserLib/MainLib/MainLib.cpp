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
		mLogger.LogInfo("--VCU Setup Failed--");
		return;
	}

	mSetupFailed = false;
	mLogger.LogInfo("--VCU Setup Finish--");
}

void Main::Loop()
{
	if (mSetupFailed)
	{
		DisplayFatalError();
		mMCUInterface.SendCommandMessageInErrorState();
		return;
	}

	// Check CAN controller's receive buffer
	mCANManager.CheckReceiveFIFO();

	// Sensor
	mSensorInterface.ReadADC(); // Read throttle and other analog signals and store it to dataStore

	mReadyToDriveManager.Check();

	// MCU
	mMCUErrorManager.CheckImplausibility(); // update implausible status
	mMCUErrorManager.CheckCommandMessageFrequency();
	mMCUErrorManager.CheckMCUTimeout();

	mMCUInterface.SendCommandMessage(); // send packet to motor controller
	mMCUInterface.StoreCommandMessageFrequency();

	mDashboardInterface.DisplayDashboard();

	// Performance test
	// mCANManager.TransmitFlood(0x0D0);
}

void Main::MailboxCompletedCallbackInvoked(const uint8_t mailboxNumber)
{
	mMCUInterface.CANMailboxCompletedCallbackHandler(mailboxNumber);
}

}

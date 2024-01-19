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
	mCanManagerForSensors.SetTransmitHeader(0x10, false);
	mCanManagerForSensors.SendMessage(txArr);

	uint8_t txArr2[8] = {1,3,5,6,9,12,14,16};
	mCanManagerForBMSAndMCU.SetTransmitHeader(0x10, false);
	mCanManagerForBMSAndMCU.SendMessage(txArr2);

//	uint8_t txArr2[8] = {1,3,5,6,9,12,14,16};
//	mCanManagerForBMSAndMCU.SetTransmitHeader(0x22, false);
//	mCanManagerForBMSAndMCU.SendMessage(txArr2);
//
//	// MCU
//	mMCUErrorManager.CheckImplausibility(); // update implausible status
//	mLogger.LogCustom("Persisted: " + std::to_string(mDataStore.GetPersistedImplausibleStatus()));
//
//	mMCUInterface.SendMessage(); // send packet to motor controller
//
//	// Ready to Drive
//	mReadyToDriveManager.CheckReadyToDrive();
}

void Main::HAL_FDCAN_RxFifo0Callback(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	// Copy the message so that the message does not get overwritten
	// TODO :: determine whether CANMessageReceiveHandlerFIFO0 message copy is necessary
	mLogger.LogInfo("TODO: is CANMessageReceiveHandlerFIFO0 message copy necessary?");
	uint8_t messageBuffer[8];
	for (int i = 0; i < 8; i++)
	{
		messageBuffer[i] = message[i];
	}

	mLogger.LogInfo("BMSAndMCUCAN receives: " + std::to_string(header.Identifier) + " " + std::to_string(message[0]) + ", " +
				std::to_string(message[1]) + ", " +
				std::to_string(message[2]) + ", " +
				std::to_string(message[3]) + ", " +
				std::to_string(message[4]) + ", " +
				std::to_string(message[5]) + ", " +
				std::to_string(message[6]) + ", " +
				std::to_string(message[7]));

	mCanManagerForBMSAndMCU.MessageReceiveHandler(header, messageBuffer);
}

void Main::HAL_FDCAN_RxFifo1Callback(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	// Copy the message so that the message does not get overwritten
	// TODO :: determine whether CANMessageReceiveHandlerFIFO0 message copy is necessary
	mLogger.LogInfo("TODO: is CANMessageReceiveHandlerFIFO1 message copy necessary?");
	uint8_t messageBuffer[8];
	for (int i = 0; i < 8; i++)
	{
		messageBuffer[i] = message[i];
	}

	mLogger.LogInfo("SensorCAN receives: " + std::to_string(header.Identifier) + " " + std::to_string(message[0]) + ", " +
			std::to_string(message[1]) + ", " +
			std::to_string(message[2]) + ", " +
			std::to_string(message[3]) + ", " +
			std::to_string(message[4]) + ", " +
			std::to_string(message[5]) + ", " +
			std::to_string(message[6]) + ", " +
			std::to_string(message[7]));

	mCanManagerForSensors.MessageReceiveHandler(header, messageBuffer);
}

}

#include <MainLib/Inc/MainLib.hpp>

namespace VehicleControlUnit { namespace MainLib {

void Main::Setup()
{
	mLogger.LogInfo("--VCU Setup Starts--");
	const auto canError = mCanManagerForBMSAndMCU.init();
	const auto adcError = mADCManager.init();

	mLogger.LogInfo("TODO: what if init failed in setup?");

	mLogger.LogInfo("--VCU Setup Finish--");
}

void Main::Loop()
{
	mLogger.LogSpam("--VCU Loop Starts--");

	// MCU
	// mMCUErrorManager.CheckImplausibility(); // update implausible status
	// mMCUInterface.SendMessage(); // send packet to motor controller

	// ADC Test
	uint16_t result1, result2, result3;
	mADCManager.GetBufferByIndex(0, result1);
	mADCManager.GetBufferByIndex(1, result2);
	mADCManager.GetBufferByIndex(2, result3);
	mLogger.LogInfo("Result " + std::to_string(result1) + " " + std::to_string(result2) + " " + std::to_string(result3));
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

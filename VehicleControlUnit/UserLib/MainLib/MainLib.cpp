#include <MainLib/Inc/MainLib.hpp>

namespace VehicleControlUnit { namespace MainLib {

void Main::Setup()
{
	mLogger.LogInfo("--VCU Setup Starts--");
	mCanManagerForBMSAndMCU.init();

	mLogger.LogInfo("--VCU Setup Finish--");
}

void Main::Loop()
{
	mLogger.LogSpam("--VCU Loop Starts--");

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

	// TEST CODE
	mLogger.LogInfo("Received: " + std::to_string(message[0]) + " " + std::to_string(message[1]) + " " + std::to_string(message[2]) + " " + std::to_string(message[3]) +
			" " + std::to_string(message[4]) + " " + std::to_string(message[5]) + " " + std::to_string(message[6]) + " " + std::to_string(message[7]));
	// TEST CODE

	mCanManagerForBMSAndMCU.MessageReceiveHandler(header, messageBuffer);
}

}}

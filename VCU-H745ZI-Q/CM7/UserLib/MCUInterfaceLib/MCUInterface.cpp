#include <MCUInterfaceLib/Inc/MCUInterface.hpp>

namespace VehicleControlUnit::MCUInterfaceLib {

UtilsLib::ErrorState MCUInterface::MessageReceiveHandler(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Handle the CAN message received from MCU, store the data in the DataStore");
	mLogger.LogCustom("MCUCAN receives: " + std::to_string(header.Identifier) + " " + std::to_string(message[0]) + ", " +
					std::to_string(message[1]));

	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

UtilsLib::ErrorState MCUInterface::SendMessage()
{
	mLogger.LogInfo("TODO: Send message to motor controller");

	// Check if the engine should be stopped
	if (mDataStore.GetPersistedImplausibleStatus())
	{
		return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_FAIL; // placeholder
	}

	// Get information from the data store (e.g. throttle, gear...)

	// Set header, send message

	// If there is error state, store it to the data store

	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

}

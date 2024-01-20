#include <BMSInterfaceLib/Inc/BMSInterface.hpp>

namespace VehicleControlUnit::BMSInterfaceLib {

UtilsLib::ErrorState BMSInterface::MessageReceiveHandler(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Handle the CAN message received from BMS, store the data in the DataStore");
	mLogger.LogCustom("BMSCAN receives: " + std::to_string(header.Identifier) + " " + std::to_string(message[0]) + ", " +
				std::to_string(message[1]));

	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

}

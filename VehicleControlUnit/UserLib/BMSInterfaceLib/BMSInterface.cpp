#include <BMSInterfaceLib/Inc/BMSInterface.hpp>

namespace VehicleControlUnit { namespace BMSInterfaceLib {

UtilsLib::ErrorState BMSInteface::MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Handle the CAN message received from BMS, store the data in the DataStore");
	return UtilsLib::ErrorState::INIT_SUCCESS; // placeholder
}

}}

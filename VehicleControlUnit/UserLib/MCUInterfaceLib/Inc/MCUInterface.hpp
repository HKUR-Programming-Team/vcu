#pragma once

#include <stm32f1xx.h>

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/CANManager.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <UtilsLib/Inc/Logger.hpp>


namespace VehicleControlUnit { namespace MCUInterfaceLib {

class MCUInteface
{
public:
	MCUInteface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, UtilsLib::CANManangerForBMSAndMCU& CANManager):
			mLogger{logger},
			mDataStore{dataStore},
			mCANManager{CANManager}
	{}

	UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);

	UtilsLib::ErrorState SendMessage();

protected:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::CANManangerForBMSAndMCU& mCANManager;
};

}};

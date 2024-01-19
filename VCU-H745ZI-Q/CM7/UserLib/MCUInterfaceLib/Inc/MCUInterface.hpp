#pragma once

#include <stm32h7xx.h>

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/CANManager.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <UtilsLib/Inc/Logger.hpp>

// Forward declaration to make circular dependency between CANManager and MCUInterface work
namespace VehicleControlUnit::UtilsLib {
	class CANManangerForBMSAndMCU;
};

namespace VehicleControlUnit::MCUInterfaceLib {

class MCUInterface
{
public:
	MCUInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, UtilsLib::CANManangerForBMSAndMCU& CANManager):
			mLogger{logger},
			mDataStore{dataStore},
			mCANManager{CANManager}
	{}

	UtilsLib::ErrorState MessageReceiveHandler(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8]);

	UtilsLib::ErrorState SendMessage();

protected:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
	UtilsLib::CANManangerForBMSAndMCU& mCANManager;
};

};

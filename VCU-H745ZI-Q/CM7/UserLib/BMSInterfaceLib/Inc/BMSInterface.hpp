#pragma once

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <UtilsLib/Inc/Logger.hpp>

#include <stm32h7xx.h>

namespace VehicleControlUnit { namespace BMSInterfaceLib {

class BMSInterface
{
public:
	BMSInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore):
			mLogger{logger},
			mDataStore{dataStore}
	{}

	UtilsLib::ErrorState MessageReceiveHandler(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8]);
protected:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
};

}};

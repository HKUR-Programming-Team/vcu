#pragma once

#include <stm32f1xx.h>

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <UtilsLib/Inc/Logger.hpp>

namespace VehicleControlUnit::BMSInterfaceLib {

class BMSInterface
{
public:
	BMSInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore):
			mLogger{logger},
			mDataStore{dataStore}
	{}

	UtilsLib::ErrorState MessageReceiveHandler(const CAN_RxHeaderTypeDef& header, const uint8_t message[8]);
protected:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;
};

};

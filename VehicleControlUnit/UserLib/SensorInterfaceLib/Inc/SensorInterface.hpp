#pragma once

#include <UtilsLib/Inc/Logger.hpp>
#include <DataStoreLib/Inc/DataStore.hpp>

namespace VehicleControlUnit { namespace SensorInterfaceLib {

class SensorInterface
{
public:

	SensorInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore):
		mLogger{logger},
		mDataStore{dataStore}
	{}

private:

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;

};

}}

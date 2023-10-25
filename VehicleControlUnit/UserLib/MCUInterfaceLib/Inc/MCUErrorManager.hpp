#pragma once

#include <stm32f1xx.h>

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <UtilsLib/Inc/Logger.hpp>

namespace VehicleControlUnit { namespace MCUInterfaceLib {

class MCUErrorManager
{
public:
	MCUErrorManager(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, const uint32_t implausibleThresholdInterval):
		mLogger{logger},
		mDataStore{dataStore},
		mInImplausibleState{false},
		mImplausibleStartTs{0},
		mImplausibleThresholdInterval{implausibleThresholdInterval}
	{}

	void CheckImplausibility();

	// To be called when error is resolved (i.e. you are ready to restart the car)
	void ResetErrorState();

private:
	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;

	bool mInImplausibleState;
	bool mImplausibleStartTs;

	const uint32_t mImplausibleThresholdInterval;

};

}}

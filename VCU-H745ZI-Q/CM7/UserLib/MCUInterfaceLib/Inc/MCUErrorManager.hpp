#pragma once

#include <stm32h7xx.h>

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>
#include <UtilsLib/Inc/Logger.hpp>

namespace VehicleControlUnit::MCUInterfaceLib {

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

	// To be called in the main library
	void CheckImplausibility();

	// To be called when error is resolved (i.e. you are ready to restart the car)
	void ResetErrorState();

private:
	bool IsCurrentlyImplausible() const;

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;

	bool mInImplausibleState;
	uint32_t mImplausibleStartTs;

	const uint32_t mImplausibleThresholdInterval;

};

}

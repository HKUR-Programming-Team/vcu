#pragma once

#ifndef MOCK_TEST
	#include <UtilsLib/Inc/CANManager.hpp>
	#include <UtilsLib/Inc/GPIOManager.hpp>
	#include <UtilsLib/Inc/Logger.hpp>
	#include <stm32f1xx.h>
#else
	#include <MockLibraries.hpp>
#endif

#include <DataStoreLib/Inc/DataStore.hpp>
#include <MainLib/Inc/Config.hpp>

namespace VehicleControlUnit::DashboardInterfaceLib {

class DashboardInterface
{
public:
	DashboardInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, const MainLib::Config::Config& config):
			mLogger{logger},
			mDataStore{dataStore},
			mParameters{config.mDashboardInterfaceConfig}
	{}

	void DisplayDashboard();
private:
	void DisplayError();

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;

	const MainLib::Config::DashboardInterfaceConfig mParameters;
};

};

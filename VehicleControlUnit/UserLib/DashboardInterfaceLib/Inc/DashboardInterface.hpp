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
#include <MainLib/Inc/settings.hpp>

namespace VehicleControlUnit::DashboardInterfaceLib {

class DashboardInterface
{
public:
	DashboardInterface(UtilsLib::Logger& logger, DataStoreLib::DataStore& dataStore, const MainLib::Settings::DashboardInterfaceParameters dashboardParameters):
			mLogger{logger},
			mDataStore{dataStore},
			mParameters{dashboardParameters}
	{}

	void DisplayDashboard();
private:
	void DisplayError();

	UtilsLib::Logger& mLogger;
	DataStoreLib::DataStore& mDataStore;

	const MainLib::Settings::DashboardInterfaceParameters mParameters;
};

};

#ifndef VCU_MAIN
#define VCU_MAIN

#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/CANManager.hpp>
#include <MainLib/Inc/setttings.hpp>

#include <stm32f1xx.h>

namespace VehicleControlUnit { namespace MainLib {

class Main
{

public:
	Main(CAN_HandleTypeDef& canHandler) :
		logger{Settings::spamLoggingEnabled,
				Settings::infoLoggingEnabled,
				Settings::errorLoggingEnabled},
		canManager(logger, canHandler, "CAN1")
	{}

	void Setup();
	void Loop();

private:
	UtilsLib::Logger logger;

	UtilsLib::CANManager canManager;
	DataStoreLib::DataStore dataStore;
};

}} // namespace VehicleControlUnit::MainLib

#endif

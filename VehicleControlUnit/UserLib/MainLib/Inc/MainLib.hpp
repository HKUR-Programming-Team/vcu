#ifndef VCU_MAIN
#define VCU_MAIN

#include <UtilsLib/Inc/Logger.hpp>
#include <MainLib/Inc/setttings.hpp>

#include <stm32f1xx.h>

namespace VehicleControlUnit { namespace MainLib {

class Main
{

public:
	Main(CAN_HandleTypeDef* const canHandler) :
		canHandler(canHandler)
	{}

	void Setup();
	void Loop();

private:
	CAN_HandleTypeDef* canHandler;

	VehicleControlUnit::UtilsLib::Logger logger{
			Settings::spamLoggingEnabled,
			Settings::infoLoggingEnabled,
			Settings::errorLoggingEnabled};
};

}} // namespace VehicleControlUnit::MainLib

#endif

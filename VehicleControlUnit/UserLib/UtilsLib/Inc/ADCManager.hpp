#pragma once

#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>

#include <stm32f1xx.h>

namespace VehicleControlUnit { namespace UtilsLib {

class ADCManager
{
public:
	ADCManager(UtilsLib::Logger& logger):
			mLogger{logger}
	{}

	ErrorState init();
private:

	UtilsLib::Logger& mLogger;

};

}};

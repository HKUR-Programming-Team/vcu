#include <MainLib/Inc/MainLib.hpp>

namespace VehicleControlUnit { namespace MainLib {

void Main::Setup()
{
	logger.LogInfo("This is an Info log");
	logger.LogSpam("This is a spam log");
	logger.LogError("This is a error log");
}

void Main::Loop()
{
	logger.LogSpam("This is a spam log bruhh");
}

}}

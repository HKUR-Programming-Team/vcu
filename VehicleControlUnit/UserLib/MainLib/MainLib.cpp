#include <MainLib/Inc/MainLib.hpp>

namespace VehicleControlUnit { namespace MainLib {

void Main::Setup()
{
	logger.LogInfo("--VCU Setup Starts--");
	canManager.init();
	canManager.SetTransmitHeader(1, 2, true, true, true);
}

void Main::Loop()
{
	logger.LogSpam("--VCU Loop Starts--");
}

}}

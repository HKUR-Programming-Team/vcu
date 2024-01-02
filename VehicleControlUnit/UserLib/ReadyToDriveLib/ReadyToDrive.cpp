#include <ReadyToDriveLib/Inc/ReadyToDrive.hpp>

namespace VehicleControlUnit { namespace ReadyToDriveLib {

void ReadyToDrive::CheckReadyToDrive()
{
	mLogger.LogInfo("TODO: check ready to drive");

	// Using information from the data store, check the ready to drive sound status
	// Determine whether should the ready to drive sound be played, and use the GPIO/CAN library to trigger ready to drive sound.
	mLogger.LogInfo("Ready to drive sound trigger: CAN or GPIO?");
}

}};

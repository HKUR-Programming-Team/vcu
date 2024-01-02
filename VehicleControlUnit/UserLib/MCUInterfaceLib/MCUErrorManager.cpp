#include <MCUInterfaceLib/Inc/MCUErrorManager.hpp>

namespace VehicleControlUnit { namespace MCUInterfaceLib {

void MCUErrorManager::CheckImplausibility()
{
	mLogger.LogInfo("TODO: Check implausible state");
	// Check if the current state is implausible now. You may need to retrieve information from the data store

	// Placeholder that represent the check result
	bool implausible = false;

	// If implausible is true and mInImplausibleState is true
		// If implausibleThresholdInterval < current system clock time - implausibleStartTs
			// set PersistedImplausibleStatus in datastore to true to indicate that error has occured
	// If implausible is true and mInImplausibleState is false
	    // set mInImplausibleState to true
		// set the mImplausibleStartTs to current system clock time
}

void MCUErrorManager::ResetErrorState()
{
	mDataStore.SetPersistedImplausibleStatus(false);
	mInImplausibleState = false;
}

}}

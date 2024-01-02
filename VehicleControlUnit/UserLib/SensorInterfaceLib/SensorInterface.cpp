#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

namespace VehicleControlUnit { namespace SensorInterfaceLib {

void SensorInterface::ReadADC()
{
	ReadThrottleSignal();
}

void SensorInterface::ReadThrottleSignal()
{
	uint16_t result1 = 0;
	uint16_t result2 = 0;

	const auto error1 = mADCManager.GetBufferByIndex(mThrottleSignalADCIndex1, result1);
	const auto error2 = mADCManager.GetBufferByIndex(mThrottleSignalADCIndex2, result2);

	if (error1 != UtilsLib::ErrorState::ADC_DMA_SUCCESS || error2 != UtilsLib::ErrorState::ADC_DMA_SUCCESS)
	{
		// should indicate failure by changing a flag in data store (so implausible state can be triggered).
		mLogger.LogInfo("TODO: SensorInterface ReadThrottleSignal failure: store failure to data store.");
		return;
	}

	mLogger.LogInfo("TODO: SensorInterface ReadThrottleSignal process and store signal to data store.");
	mLogger.LogInfo("Read signals: " + std::to_string(result1) + ", " + std::to_string(result2));
	return;
}

}}

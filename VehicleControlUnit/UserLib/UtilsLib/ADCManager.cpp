#include <UtilsLib/Inc/ADCManager.hpp>

namespace VehicleControlUnit { namespace UtilsLib {

ErrorState ADCManager::init()
{
	mLogger.LogInfo("Initialization of ADC Manager Starts");
	auto error = HAL_ADCEx_Calibration_Start(&mADCHandler);
	if (error == HAL_ERROR)
	{
		mLogger.LogError("Initialization of HAL_ADCEx_Calibration_Start failed. Please raise to developers");
		return ErrorState::INIT_FAIL;
	}

	error = HAL_ADC_Start_DMA(&mADCHandler, (uint32_t *) mDataBuffer, mBufferLength);
	if (error == HAL_ERROR)
	{
		mLogger.LogError("Initialization of HAL_ADC_START_DMA failed. Please raise to developers");
		return ErrorState::INIT_FAIL;
	}

	mLogger.LogInfo("Initialization of ADC Manager Success");
	return ErrorState::INIT_SUCCESS; // Placeholder
}

ErrorState ADCManager::GetBufferByIndex(const uint8_t index, uint16_t& result) const
{
	if (index < 0 || index >= mBufferLength)
	{
		mLogger.LogError("Index out of range while accessing ADC DMA buffer. Index: " + std::to_string(index) + ". Please raise to developers");
		return ErrorState::ADC_DMA_INDEX_OUT_OF_RANGE;
	}

	result = mDataBuffer[index];
	return ErrorState::ADC_DMA_SUCCESS;
}

uint8_t ADCManager::GetBufferLength() const
{
	return mBufferLength;
}

}}

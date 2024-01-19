#pragma once

#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/ErrorState.hpp>

#include <stm32h7xx.h>

namespace VehicleControlUnit::UtilsLib {

class ADCManager
{
public:
	ADCManager(UtilsLib::Logger& logger, ADC_HandleTypeDef& adcHandler, const std::string& adcName, const uint32_t bufferLength):
			mLogger{logger},
			mADCHandler{adcHandler},
			mBufferLength{bufferLength}
	{}

	ErrorState init();

	ErrorState GetBufferByIndex(const uint8_t index, uint16_t& result) const;
	uint8_t GetBufferLength() const;
private:
	UtilsLib::Logger& mLogger;
	ADC_HandleTypeDef& mADCHandler;

	const uint32_t mBufferLength;
	uint16_t mDataBuffer[16];
};

};

#pragma once

#include <UtilsLib/Inc/Logger.hpp>

#include <stm32f1xx.h>

namespace VehicleControlUnit::UtilsLib {

enum class GPIOPort
{
	A,
	B,
	C,
	D,
	E
};

enum class GPIOPinNum
{
	Pin0,
	Pin1,
	Pin2,
	Pin3,
	Pin4,
	Pin5,
	Pin6,
	Pin7,
	Pin8,
	Pin9,
	Pin10,
	Pin11,
	Pin12,
	Pin13,
	Pin14,
	Pin15
};

class GPIOManager
{
public:

	static const bool digitalRead(const GPIOPort& alphabet, const GPIOPinNum& pinNumber);
	static void digitalWrite(const GPIOPort& alphabet, const GPIOPinNum& pinNumber, const bool value);

private:
	static GPIO_TypeDef* GPIOAlphabetToHALTypeDef(const GPIOPort& gpioAlphabet);
	static uint32_t GPIOPinToHALPin(const GPIOPinNum& gpioPinNum);

};

}

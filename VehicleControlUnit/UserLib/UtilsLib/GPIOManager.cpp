#include <UtilsLib/Inc/GPIOManager.hpp>

namespace VehicleControlUnit::UtilsLib {

const bool GPIOManager::digitalRead(const GPIOPort& alphabet, const GPIOPinNum& pinNumber)
{
	auto HALportTypeDef = GPIOAlphabetToHALTypeDef(alphabet);
	const auto& HALpinNum = GPIOPinToHALPin(pinNumber);

	const auto result = HAL_GPIO_ReadPin(HALportTypeDef, HALpinNum);

	return result == GPIO_PIN_SET ? true : false;
}

void GPIOManager::digitalWrite(const GPIOPort& alphabet, const GPIOPinNum& pinNumber, const bool high)
{
	auto HALportTypeDef = GPIOAlphabetToHALTypeDef(alphabet);
	const auto& HALpinNum = GPIOPinToHALPin(pinNumber);
	const auto& HALpinState = high ? GPIO_PIN_SET : GPIO_PIN_RESET;

	HAL_GPIO_WritePin(HALportTypeDef, HALpinNum, HALpinState);
}

GPIO_TypeDef* GPIOManager::GPIOAlphabetToHALTypeDef(const GPIOPort& gpioAlphabet)
{
	switch (gpioAlphabet)
	{
		case GPIOPort::A: return GPIOA; break;
		case GPIOPort::B: return GPIOB; break;
		case GPIOPort::C: return GPIOC; break;
		case GPIOPort::D: return GPIOD; break;
		case GPIOPort::E: return GPIOE; break;
	}
}

uint32_t GPIOManager::GPIOPinToHALPin(const GPIOPinNum& gpioPinNum)
{
	switch (gpioPinNum)
	{
		case GPIOPinNum::Pin0: return GPIO_PIN_0; break;
		case GPIOPinNum::Pin1: return GPIO_PIN_1; break;
		case GPIOPinNum::Pin2: return GPIO_PIN_2; break;
		case GPIOPinNum::Pin3: return GPIO_PIN_3; break;
		case GPIOPinNum::Pin4: return GPIO_PIN_4; break;
		case GPIOPinNum::Pin5: return GPIO_PIN_5; break;
		case GPIOPinNum::Pin6: return GPIO_PIN_6; break;
		case GPIOPinNum::Pin7: return GPIO_PIN_7; break;
		case GPIOPinNum::Pin8: return GPIO_PIN_8; break;
		case GPIOPinNum::Pin9: return GPIO_PIN_9; break;
		case GPIOPinNum::Pin10: return GPIO_PIN_10; break;
		case GPIOPinNum::Pin11: return GPIO_PIN_11; break;
		case GPIOPinNum::Pin12: return GPIO_PIN_12; break;
		case GPIOPinNum::Pin13: return GPIO_PIN_13; break;
		case GPIOPinNum::Pin14: return GPIO_PIN_14; break;
		case GPIOPinNum::Pin15: return GPIO_PIN_15; break;

	}
}

}

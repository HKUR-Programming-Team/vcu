#pragma once

#include <UtilsLib/Inc/ErrorState.hpp>
#include <stdint.h>
#include <string>

struct CAN_RxHeaderTypeDef
{
    uint32_t StdId;
};

namespace VehicleControlUnit::UtilsLib {

class CANManager
{
public:

	void SetTransmitHeader(
			const uint32_t messageIdentifier,
			const uint32_t messageLength,
			const bool identifierExtension = false,
			const bool isRemoteTransmissionRequest = false,
			const bool transmitGlobalTime = false)
    {
        mMessageId = messageIdentifier;
        mMessageLength = messageLength;
        return;
    }

	ErrorState SendMessage(const uint8_t message[8])
    {
        for (int i = 0; i < 8; ++i)
        {
            buffer[i] = message[i];
        }
        return ErrorState::INIT_SUCCESS;
    }

	ErrorState CheckReceiveFIFO()
    {
        volatile int x = 0;
        return ErrorState::INIT_SUCCESS;
    }

    uint32_t mMessageId = 0;
    uint32_t mMessageLength = 0;
    uint8_t buffer[8];
    bool SendMessageError = false;
};

class ADCManager
{
public:
	ErrorState GetBufferByIndex(const uint8_t index, uint16_t& result) const
    {
        result = buffer[index];
        return ErrorState::INIT_SUCCESS;
    }

	uint8_t GetBufferLength() const
    {
        volatile int x = 0;
        return 0;
    }

    uint16_t buffer[16];
};

class Logger
{
public:
	// Interfaces for logging
	void LogSpam(const std::string & logMessage) const
    {
        volatile int x = 0;
    }

	void LogInfo(const std::string & logMessage) const
    {
        volatile int x = 0;
    }

	void LogError(const std::string & logMessage) const
    {
        volatile int x = 0;
    }

	void LogCustom(const std::string & logMessage) const
    {
        volatile int x = 0;
    }
};

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

	static const bool digitalRead(const GPIOPort& alphabet, const GPIOPinNum& pinNumber)
    {
        if (alphabet == GPIOPort::A && pinNumber == GPIOPinNum::Pin0)
        {
            return A0;
        }
        if (alphabet == GPIOPort::A && pinNumber == GPIOPinNum::Pin1)
        {
            return A1;
        }
        if (alphabet == GPIOPort::B && pinNumber == GPIOPinNum::Pin0)
        {
            return B0;
        }
        if (alphabet == GPIOPort::B && pinNumber == GPIOPinNum::Pin1)
        {
            return B1;
        }

        return false;
    }
	static void digitalWrite(const GPIOPort& alphabet, const GPIOPinNum& pinNumber, const bool value)
    {
        if (alphabet == GPIOPort::A && pinNumber == GPIOPinNum::Pin0)
        {
            A0 = value;
        }
        if (alphabet == GPIOPort::A && pinNumber == GPIOPinNum::Pin1)
        {
            A1 = value;
        }
        if (alphabet == GPIOPort::B && pinNumber == GPIOPinNum::Pin0)
        {
            B0 = value;
        }
        if (alphabet == GPIOPort::B && pinNumber == GPIOPinNum::Pin1)
        {
            B1 = value;
        }
    }

    static bool A0;
    static bool A1;
    static bool B0;
    static bool B1;
};

}

inline extern uint32_t MockCurrentTick = 0;

inline uint32_t HAL_GetTick()
{
    return MockCurrentTick;
}
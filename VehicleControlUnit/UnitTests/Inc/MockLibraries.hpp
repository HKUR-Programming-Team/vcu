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
        volatile int x = 0;
        return;
    }

	ErrorState SendMessage(const uint8_t message[8])
    {
        volatile int x = 0;
        return ErrorState::INIT_SUCCESS;
    }

	ErrorState CheckReceiveFIFO()
    {
        volatile int x = 0;
        return ErrorState::INIT_SUCCESS;
    }
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

}

inline extern uint32_t MockCurrentTick = 0;

inline uint32_t HAL_GetTick()
{
    return MockCurrentTick;
}